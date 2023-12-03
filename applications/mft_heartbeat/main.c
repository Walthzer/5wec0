 /*
 *  TU/e 5WEC0::PYNQ RYB Module Testing. Heartbeat module
 *
 *  Interface:
 *  Switch 1 -> Exits the main loop
 *  Switch 0: -> SQUARE/ECG MODE
 *  Button 3: NC
 *  Button 2: NC
 *  Button 1: NC
 *  Button 0: NC
 * 
 *  Written by: Walthzer
 * 
 */
#include <libpynq.h>
#include <stdlib.h>
#include <time.h>
#include <libui.h>
#include <libcom.h>

#define MIN(a,b) a > b ? b : a
#define MAX(a,b) a > b ? a : b

#define SENSOR_PIN IO_A0
#define TEST_PIN IO_AR7

#define PULSE_COUNT 2
#define MIN_PERIOD_MS 5

typedef struct _heartbeat_data_
{
  float frequency;
  uint32_t bpm;
} Data;

typedef struct _pulse_
{
  clock_t rise, fall;
  int valid;
} Pulse;

typedef struct _signal_
{
  Pulse pulses[PULSE_COUNT]; //Falling Edge timestamps
  int state, next_pulse;
} Signal;

enum SIGNAL_EVENT {NONE, RISE, FALL}; 

//Globals
//FLAGS
enum CRADLE_MODES {SQUARE, ECG};
static int Cradle_Mode = -1; 

//Calculate ms period from rise and fall stamps
//Results is always positive
float period_ms(clock_t a, clock_t b)
{
  return ((a > b ? a - b : b - a) / CLOCKS_PER_SEC) * 1000;
}

void process_input(Signal *signal)
{
  int event = NONE;
  clock_t t_curr = clock();

  
  if(!gpio_get_level(SENSOR_PIN))
  { 
    //Input LOW
    //Is Falling edge?
    if(signal->state)
    {
      printf("Fall\n");
      event = FALL;
      signal->state = 0;
    }

  } else
  {
    //Input HIGH
    //Is Rising edge?
    if(!signal->state)
    {
      printf("Rise\n");
      event = RISE;
      signal->state = 1;
    }
  }

  

  if(Cradle_Mode == ECG)
  {
    return;
  }

  //SQUARE MODE
  if(Cradle_Mode == SQUARE)
  {
    Pulse *pulse = &signal->pulses[signal->next_pulse];

    //No event -> Check for glitch timeout
    if(event == NONE)
    {
      //Pulse incomplete or valid?
      if(!pulse->valid || pulse->fall == 0 || pulse->rise == 0)
        return;

      //Delta between previous pulse event below MIN_PERIOD_MS?
      if(period_ms(t_curr, MAX(pulse->fall, pulse->rise)) < MIN_PERIOD_MS)
        return;


      //Unmark All pulses
      for (int i = 0; i < PULSE_COUNT; i++)
      {
        signal->pulses[i].valid = 0;
      }

      //Mark this pulse valid
      pulse->valid = 1;
      printf("Event_Timeout->valid");

      return;
    } 

    //Complete and invalid signal?
    if(false && !pulse->valid && pulse->fall != 0 && pulse->rise != 0)
    {
      

      //Pointer to last event time
      clock_t *ptr_t_event;
      if(pulse->fall > pulse->rise)
        ptr_t_event = &pulse->fall;
      else
        ptr_t_event = &pulse->rise;

      //if delta of events below MIN_PERIOD_MS then discard event;
      printf("PERIOD: %f\n", period_ms(t_curr, *ptr_t_event));
      if(period_ms(t_curr, *ptr_t_event) < MIN_PERIOD_MS)
      {
        printf("Glitch->discard\n");
        *ptr_t_event = 0; //Previous event was also part of glitch
        return;
      }

      //Event is good
      //Close off signal
      pulse->valid = 1;

      signal->next_pulse++;
      if(signal->next_pulse >= PULSE_COUNT) {signal->next_pulse = 0;}
    }

    printf("Event: %d\n", event);
    //Event is good
    //Store time in pulse
    if(event == RISE)
    {
      signal->pulses[signal->next_pulse].rise = t_curr;
      gpio_set_level(TEST_PIN, GPIO_LEVEL_HIGH);
    }
    
    if(event == FALL)
    {
      signal->pulses[signal->next_pulse].fall = t_curr;
      gpio_set_level(TEST_PIN, GPIO_LEVEL_LOW);
    }
  }
}

int main(void) {
  pynq_init();
  ui_t ui;
  ui_init(&ui);
  display_set_flip(&ui.display, false, false);

  //Set header
  ui_rcenter(&ui, 0, true);
  ui_rprintf(&ui, 0, "%q%w%tRYB 24", RGB_WHITE, RGB_ORANGE, COMIC);
  ui_rcenter(&ui, 1, true);
  ui_rprintf(&ui, 1, "%q%w%tHeartbeat", RGB_WHITE, RGB_ORANGE, COMIC);
  //interface layout
  //Mode: Square/ECG
  ui_rprintf(&ui, 2, "%q--Setup--", RGB_CYAN);
  ui_rcenter(&ui, 2, true);
  ui_rprintf(&ui, 3, "%qMode: %q%s", RGB_ORANGE, RGB_RED, "???");
  //Outputs: Heartbeat
  ui_rprintf(&ui, 4, "%q--Outputs--", RGB_CYAN);
  ui_rcenter(&ui, 4, true);

  ui_draw(&ui);

  com_t com;
  com_init(&com, HEARTBEAT);

  //Init inputs
  gpio_init();
  switches_init();
  buttons_init();
  
  //Interrupt init
  gpio_interrupt_init();
  gpio_set_direction(SENSOR_PIN, GPIO_DIR_INPUT);
  gpio_set_direction(TEST_PIN, GPIO_DIR_OUTPUT);

  //----Init Vars---
  Data data = {0, 0};
  //---Signal struct---
  Signal signal;
  signal.state = gpio_get_level(SENSOR_PIN);
  signal.next_pulse = 0;
  for (int i = 0; i < PULSE_COUNT; i++)
  {
    signal.pulses[i] = (Pulse){0, 0, 0};
  }
  //---END Signal---
  //---END Vars---
  

  while (get_switch_state(1))
  { 
    Cradle_Mode = get_switch_state(0);
    ui_rprintf(&ui, 3, "%qMode: %q%s", RGB_ORANGE, RGB_RED, Cradle_Mode ? "ECG" : "Square");

    process_input(&signal);

    //Find valid pulse
    Pulse *valid_pulse = NULL;
    for (int i = 0; i < PULSE_COUNT; i++)
    {
      if(signal.pulses[i].valid)
      {
        valid_pulse = &signal.pulses[i];
        break;
      }
    }

    if(valid_pulse != NULL)
    {
      float delta_ms = period_ms(valid_pulse->rise, valid_pulse->fall);
      data.frequency = 1000.0f / delta_ms;
      data.bpm = 60 * data.frequency;
      //printf("---------PERIOD: %f ---------\n", data.frequency);
      fflush(stdout);
    }
  
    //outputs
    //com_put(&com, HEARTBEAT, data.bpm);
    //com_get(&com, HEARTBEAT, &data.bpm);
    
    ui_rprintf(&ui, 6, "%qFrequency: %q%f", RGB_PURPLE, RGB_GREEN, data.frequency);
    ui_rprintf(&ui, 7, "%qBPM:[60-240]: %q%d", RGB_PURPLE, RGB_GREEN, data.bpm);

    ui_draw(&ui);
    com_run(&com);
  }

  buttons_destroy();
  switches_destroy();
  com_destroy(&com);
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};