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

#include <unistd.h>
#include <libpynq.h>
#include <stdlib.h>
#include <sys/time.h>
#include <libui.h>
#include <libcom.h>
#include <pthread.h>
#include <stdatomic.h>

#define DEBUG

#define MIN(a,b) a > b ? b : a
#define MAX(a,b) a > b ? a : b

#define SENSOR_PIN IO_A0
#define TEST_PIN IO_AR7

#define PULSE_COUNT 2
#define MIN_PERIOD_MS 100
#define GLITCH_PERIOD 10
#define MIN_READINGS 4

#define FREE_PTR(ptr) free(ptr); ptr = NULL
#undef LOG_DOMAIN
#define LOG_DOMAIN "mft_heartbeat"

struct thread_arg
{
  ui_t* ui;
  com_t* com;
};

enum SIGNAL_EVENT {NONE, RISE, FALL}; 

typedef struct _pulse_event_
{
  int type;
  struct timeval time;
} Event;

typedef struct _pulse_
{
  Event events[2];
  int valid;
} Pulse;

typedef struct _signal_
{
  Pulse pulses[PULSE_COUNT]; //Falling Edge timestamps
  int state, curr_pulse;
  int period;
  int readings[MIN_READINGS];
  int idx_reading;
} Signal;

//Globals
//FLAGS
enum CRADLE_MODES {SQUARE_VARIATION, SQUARE, ECG, ECG_VARIATION};
static int Cradle_Mode = -1; 

//PERIOD
atomic_int period_atom;
atomic_bool output_kalive;

//Get the truth value of the sensor
bool get_sensor_state()
{
  float state = adc_read_channel_raw(ADC0);
  //printf("State: %f\n", state);
  return state > 2000;
}

//Calculate ms period from rise and fall stamps
//Results is always positive
int signed_time_diff_ms(struct timeval *a, struct timeval *b)
{
  return ((a->tv_sec - b->tv_sec) + 1e-6 * (a->tv_usec - b->tv_usec)) * 1000.0f;
}


int time_diff_ms(struct timeval *a, struct timeval *b) {

  int delta = signed_time_diff_ms(a, b);
  if (delta < 0)
    return -delta;
  return delta;
}

void consume_pulse(Signal *signal, Pulse *pulse)
{
  if(Cradle_Mode >= ECG)
  {
    return;
  }

  if(Cradle_Mode <= SQUARE)
  {
    signal->readings[0] = time_diff_ms(&pulse->events[0].time, &pulse->events[1].time);
    clear_pulse(pulse);
  }
}

int pulse_full(Pulse *pulse)
{
  return (pulse->events[0].type != NONE && pulse->events[1].type != NONE);
}

void clear_pulse_event(Event* event)
{
  event->type = NONE;
  event->time = (struct timeval){0, 0};
}

void clear_pulse(Pulse* pulse)
{
  pulse->valid = 0;
  clear_pulse_event(&pulse->events[0]);
  clear_pulse_event(&pulse->events[1]);
}

void process_input(Signal *signal)
{
  int event = NONE;
  struct timeval t_curr = {0, 0};
  gettimeofday(&t_curr, NULL);
  
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

  if(Cradle_Mode >= ECG)
  {
    return;
  }

  //SQUARE MODE
  if(Cradle_Mode <= SQUARE)
  {
    /*Signal
      -> can be split by glitches
      -> have a very close leading or tailing glitch (Broke the last code)
      
      fix:
        track two pulses at a time -> combine if both MIN_PERIOD_MS
        if either is glitch -> glitch gets discarded.
        -> combination is rolling so more then 1 split is taken care of.
    */
    Pulse *pulse = &signal->pulses[signal->curr_pulse];
    int delta_ms;

    //No event
    //Check for invalid period
    //Combine split pulses
    if(event == NONE)
    {
      //Check timeout on possible leading pulse
      if(signal->curr_pulse > 0)
      {
        delta_ms = time_diff_ms(&t_curr, &pulse->events[1].time);
        if(delta_ms > GLITCH_PERIOD)
        {
          //Signal is stable after pulse
          //pulse period is valid
          //WARNING: Clearing everythin now. No idea if that is good
          consume_pulse(signal, &signal->pulses[0]);
          clear_pulse(&signal->pulses[1]);
          signal->curr_pulse = 0;
          return;
        }
      }

      //Incomplete pulses are not of intrest
      if(pulse_full(pulse))
        return;

      //Is period less then MIN_PERIOD_MS
      //-> pulse is glitch
      //any previous pulse has valid period.
      delta_ms = time_diff_ms(&pulse->events[0].time, &pulse->events[1].time);
      if(delta_ms < MIN_PERIOD_MS)
      {
        //Discard pulse
        printf("---Clear GLITCH---\n");
        clear_pulse(pulse);
        //Try to calculate period
        //Is there a leading pulse?
        if(signal->curr_pulse == 0)
          return; //No 

        printf("Leading pulse valid\n");
        consume_pulse(signal, &signal->pulses[0]);
        signal->curr_pulse = 0;
        return;
      }

      //Try to join
      //is there a leading pulse?
      if(signal->curr_pulse == 0)
        return; //No 
      
      //We have 2 valid pulses -> is delta < GLITCH_PERIOD?
      //delta between last event of pulse 0 and first event of pulse 1
      delta_ms = time_diff_ms(&signal->pulses[0].events[1].time, &pulse->events[0].time);
      if(delta_ms < GLITCH_PERIOD)
      {
        //Join pulses
        signal->pulses[0].events[1] = pulse->events[1];
        clear_pulse(&pulse);
        //Next steps:
        //-> more valid pulses to join
        // OR pulse period becomes valid due to:
        //-> pulse gets timed out
        //-> next pulses is a GLITCH 
        return;
      }

      //Two valid pulses seperated by delta > GLITCH_PERIOD?
      //PANIC
      pynq_warning("Event: NONE; PANIC. Adjust glitch period??");
      clear_pulse(&signal->pulses[0]);
      clear_pulse(&signal->pulses[1]);
      signal->curr_pulse = 0;
      return;
    }

    //Event is good
    //Store event -> 0 or 1 depending on what is filled
    int idx_event = pulse->events[signal->curr_pulse].type != NONE;
    pulse->events[idx_event].type = event;
    pulse->events[idx_event].time = t_curr;

    //Go to next pulse if we just filled up the pulse
    if(pulse_full(pulse))
      signal->curr_pulse++;


    #ifdef DEBUG
    if(event == RISE)
    {
      printf("RISE");
      gpio_set_level(TEST_PIN, GPIO_LEVEL_HIGH);
    }
    
    if(event == FALL)
    {
      printf("FALL");
      gpio_set_level(TEST_PIN, GPIO_LEVEL_LOW);
    }
    #endif
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
    
    double period;
    struct timespec t_sleep = {0, 10000};
    int mode = Cradle_Mode;

    while(atomic_load(&output_kalive))
    {
      double new_period = atomic_load(&period_atom);
      if(new_period == period && Cradle_Mode == mode)
      {
        nanosleep(&t_sleep, NULL); //sleep in microseconds
        continue;
      }
      mode = Cradle_Mode;
      period = new_period;
      double frequency = 500.0 / period;
      int bpm = 60 * (500.0 / period); //150ms -> 200  // 1000ms -> 30  //500ms -> 60

      printf("---------PERIOD: %f ---------\n", period);
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
  signal.curr_pulse = 0;
  signal.period = 0;
  for (int i = 0; i < MIN_READINGS; i++)
  {
      signal.readings[i] = 0;
  }

  for (int i = 0; i < PULSE_COUNT; i++)
  {
    signal.pulses[i].valid = 0;
    signal.pulses[0].events[0] = (Event){NONE, {0,0}};
    signal.pulses[0].events[1] = (Event){NONE, {0,0}};
  }
  //---END Signal---
  //---END Vars---

  //Threading
  atomic_init(&period_atom, 50);
  atomic_init(&output_kalive, true);
  signal.period = atomic_load(&period_atom);
  struct thread_arg thrd_args = {&ui, &com};
  pthread_t output_thread;
  pthread_create(&output_thread, NULL, &fnc_output_thread, &thrd_args);

  
  while (get_switch_state(1))
  { 
    Cradle_Mode = get_switch_state(0);

    process_input(&signal);

    //Mode depended pulse handeling
    if(Cradle_Mode == SQUARE || Cradle_Mode == ECG)
    {
      if(signal.readings[0] != signal.period)
      {
        signal.period = signal.readings[0];
        printf("---------PERIOD: %d ---------\n", signal.period);
        fflush(NULL);
        atomic_store(&period_atom, signal.period);
      }
      continue;
    }

    if(Cradle_Mode == SQUARE_VARIATION || Cradle_Mode == ECG_VARIATION)
    {
      continue;
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