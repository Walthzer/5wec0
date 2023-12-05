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

#include <pthread.h>
#include <stdatomic.h>

#define MIN(a,b) a > b ? b : a
#define MAX(a,b) a > b ? a : b

#define SENSOR_PIN IO_A0
#define TEST_PIN IO_AR7

#define PULSE_COUNT 2
#define MIN_PERIOD_MS 5

struct thread_arg
{
  ui_t* ui;
  com_t* com;
};

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

//PERIOD
atomic_int_fast16_t period_atom;
atomic_bool output_kalive;
int16_t period;

//Get the truth value of the sensor
bool get_sensor_state()
{
  float state = adc_read_channel_raw(ADC0);
  //printf("State: %f\n", state);
  return state > 2000;
}

//Calculate ms period from rise and fall stamps
//Results is always positive
long double period_ms(clock_t a, clock_t b)
{
  return ((long double)((a > b ? a - b : b - a)*1000 / (clock_t)(CLOCKS_PER_SEC)));
}

void process_input(Signal *signal)
{
  int event = NONE;
  clock_t t_curr = clock();

  
  if(!get_sensor_state())
  { 
    //Input LOW
    //Is Falling edge?
    if(signal->state)
    {
      //printf("Fall\n");
      event = FALL;
      signal->state = 0;
    }

  } else
  {
    //Input HIGH
    //Is Rising edge?
    if(!signal->state)
    {
      //printf("Rise\n");
      event = RISE;
      signal->state = 1;
    }
  }
  //fflush(stdout);

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
      return;
    } 

    //Complete and invalid signal?
    if((!pulse->valid) && pulse->fall != 0 && pulse->rise != 0)
    {

    }

    //printf("Event: %d\n", event);
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

void* fnc_output_thread(void* arg)
{
    struct thread_arg* thrd_args = arg;
    ui_t* ui = thrd_args->ui;
    com_t* com = thrd_args->com;

    //outputs
    //com_put(&com, HEARTBEAT, data.bpm);
    //com_get(&com, HEARTBEAT, &data.bpm);
    
    while(atomic_load(&output_kalive))
    {
      double period_local = atomic_load(&period_atom);
      double frequency = 1000.0 / period_local;
      int bpm = 60 * (500.0 / period_local); //150ms -> 200  // 1000ms -> 30  //500ms -> 60

      printf("---------PERIOD: %f ---------\n", period_local);
      ui_rprintf(ui, 3, "%qMode: %q%s", RGB_ORANGE, RGB_RED, Cradle_Mode ? "ECG" : "Square");
      ui_rprintf(ui, 6, "%qFrequency: %q%f", RGB_PURPLE, RGB_GREEN, frequency);
      ui_rprintf(ui, 7, "%qBPM:[60-240]: %q%d", RGB_PURPLE, RGB_GREEN, bpm);

      ui_draw(ui);
      com_run(com);
    }

    return NULL;
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
  gpio_set_direction(SENSOR_PIN, GPIO_DIR_INPUT);
  gpio_set_direction(TEST_PIN, GPIO_DIR_OUTPUT);


  //ADC init
  adc_init();

  //---Signal struct---
  Signal signal;
  signal.state = get_sensor_state();
  signal.next_pulse = 0;
  for (int i = 0; i < PULSE_COUNT; i++)
  {
    signal.pulses[i] = (Pulse){0, 0, 0};
  }
  //---END Signal---
  //---END Vars---

  //Threading
  atomic_init(&period_atom, 50);
  atomic_init(&output_kalive, true);
  period = atomic_load(&period_atom);
  struct thread_arg thrd_args = {&ui, &com};
  pthread_t output_thread;
  pthread_create(&output_thread, NULL, &fnc_output_thread, &thrd_args);

  
  while (get_switch_state(1))
  { 
    Cradle_Mode = get_switch_state(0);

    process_input(&signal);

    //Find valid pulse
    Pulse *valid_pulse = NULL;
    for (int i = 0; i < PULSE_COUNT; i++)
    {
      if(signal.pulses[i].rise > 0.0f && signal.pulses[i].fall > 0.0f)
      {
        valid_pulse = &signal.pulses[i];
        valid_pulse->valid = 0;
        break;
      }
    }

    if(valid_pulse != NULL)
    {
      int delta_ms = (int)period_ms(valid_pulse->rise, valid_pulse->fall);
      if(delta_ms != period)
      {
        period = delta_ms;
        atomic_store(&period_atom, period);
      };
      
    }
  }

  //threading 
  //gracefull exit from thread;
  atomic_store(&output_kalive, false);
  pthread_join(output_thread, NULL);

  //ADC
  adc_destroy();

  buttons_destroy();
  switches_destroy();
  com_destroy(&com);
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};