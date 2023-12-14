 /*
 *  TU/e 5WEC0::PYNQ RYB Module Testing. Heartbeat module
 *
 *  Interface:
 *  Switch 1 -> Exits the main loop
 *  Switch 0:
 *  Button 3: NC
 *  Button 2: NC
 *  Button 1: Change mode down
 *  Button 0: Change mode up
 * 
 *  Written by: Walthzer
 * 
 */



  /*
    ECG:
      5 BPM   => 2 seconds
      30 BPM  => 2 seconds      Glitch => 20ms?
      60 BPM  => 1 seconds      Glitch => 10ms
      125 BPM => 0.480 seconds  Glitch => 5ms
      200 BPm => 0.300 seconds  Glitch => 3ms
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
#define GLITCH_PIN IO_AR8
#define JOIN_PIN IO_AR9

#define PULSE_COUNT 2
#define MIN_READINGS 3
#define MIN_READINGS_VAR 10

//Times are in microseconds
#define MAX_PERIOD_SQUARE (1050 * 1000) //Maximum width of a SQUARE period
#define MIN_PERIOD_SQUARE (100 * 1000) //Minium width of a SQUARE period
#define MIN_PERIOD_ECG (50 * 1000)
#define GLITCH_PERIOD_30 (12.5 * 1000)
#define GLITCH_RANGE 500 //Bounds of the glitch period range. 

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
  int readings[MIN_READINGS_VAR];
  int idx_reading;
} Signal;

//Globals
//FLAGS
char *Cradle_Modes_Str[] = {"Square     ", "ECG        ", "Square Vari", "ECG Vari   "};
enum CRADLE_MODES {SQUARE, ECG, SQUARE_VARIATION, ECG_VARIATION};
static int Cradle_Mode = 0;

//PERIOD
int Glitch_Period = GLITCH_PERIOD_30;
atomic_int cradle_atom;
atomic_int period_atom;
atomic_bool output_kalive;

//Get the truth value of the sensor
bool get_sensor_state()
{
  int state = adc_read_channel_raw(ADC0);
  //printf("State: %f\n", state);
  return state > 10000;
}

//lil sorter
void sort_readings(int data[], int length)
{
  int swaps = 1;
  int buffer;

  //If we preformend any swaps -> run sort again
  //Sorting stops when we perform no swaps anymore. 
  while (swaps > 0)  
  {
    swaps = 0;
    for (int i = 0; i < length - 1; i++)
    {

      if(data[i] >= data[i + 1])
        continue;

      //Copy element A into storage
      //Set element A to equal B
      //Restore copy of A into B
      buffer = data[i];
      data[i] = data[i + 1];
      data[i + 1] = buffer;

      swaps++;
    }
  }
}

//Calculate microseconds period from rise and fall stamps
//Results is always positive
int signed_time_diff_micro(struct timeval *a, struct timeval *b)
{
  return (((a->tv_sec - b->tv_sec) * 1e+6) + (a->tv_usec - b->tv_usec));
}
int time_diff_micro(struct timeval *a, struct timeval *b) {

  int delta = signed_time_diff_micro(a, b);
  if (delta < 0)
    return -delta;
  return delta;
}
bool within_range(int a, int b, int range)
{
  //printf("In range?:\n -> a: %d\n -> b: %d +- %d\n -> y/n: %d", a, b, range, (a < b+range) && (a > b-range));
  return ((a < b+range) && (a > b-range));
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
void validate_pulse_square(Signal *signal, Pulse *pulse)
{
  if(!pulse_full(pulse))
  {
    pynq_error("Trying to validate a non-full pulse??");
    return;
  }

  int delta_micro = time_diff_micro(&pulse->events[0].time, &pulse->events[1].time);
  clear_pulse(pulse);
  //Prevent Strage periods from leaking through
  if(delta_micro < MAX_PERIOD_SQUARE && delta_micro > MIN_PERIOD_SQUARE)
  {
    if(Cradle_Mode == SQUARE)
    {
      if(signal->idx_reading >= MIN_READINGS)
      {
        return;//Prevent out-of-bounds
      }
      signal->readings[signal->idx_reading] = delta_micro;
      signal->idx_reading++;
      return;
    }

    if(Cradle_Mode == SQUARE_VARIATION)
    {
      if(signal->idx_reading >= MIN_READINGS)
      {
        signal->idx_reading = 0;
      }
      signal->readings[signal->idx_reading] = delta_micro;
      signal->idx_reading++;
      return;
    }

  }
}


struct timeval start = {0, 0};
void process_ecg(struct timeval t_curr, Signal *signal)
{
  int x = time_diff_micro(&t_curr, &start);
  int sample = adc_read_channel_raw(ADC0);
  printf("%d,%d\n", x, sample); //Output CSV
}

void process_square(struct timeval t_curr, Signal *signal)
{
  int event = NONE;
  int delta_micro;
  Pulse *pulse = &signal->pulses[signal->curr_pulse];

  if(!get_sensor_state())
  { 
    //Input LOW
    //Is Falling edge?
    if(signal->state)
    {
      //printf("Fall\n");
      event = FALL;
      signal->state = 0;
      //Look ahead to filter glitch
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

  //Wait for another event, then check its delta:
  // delta less then GLITCH_PERIOD?
  //    -> Part of a pulse (split by a glitch)
  // delta EQUAL to GLITCH_PERIOD?
  //    -> It is a glitch
  // delta GREATER to GLITCH_PERIOD?
  //    -> Part of a pulse (split by a glitch) OR a full pulse

  if(event == RISE)
  {
    printf("RISE");
    pulse->events[0].type = event;
    pulse->events[0].time = t_curr;

    gpio_set_level(TEST_PIN, GPIO_LEVEL_HIGH);
    gpio_set_level(GLITCH_PIN, GPIO_LEVEL_HIGH);
    gpio_set_level(JOIN_PIN, GPIO_LEVEL_HIGH);
  }

  if(event == FALL)
  {
    printf("FALL");
    pulse->events[1].type = event;
    pulse->events[1].time = t_curr;

    gpio_set_level(TEST_PIN, GPIO_LEVEL_LOW);
  }
  //Process a full pulse
  //Glitches are removed and leading pulses declared valid.
  //Valid pulses are when possible joined with leading pulses (gap less then MIN_PERIOD_SQUARE)
  //    -> IF JOINING IS NOT POSSIBLE => Declare leading pulse valid and shift left
  if(pulse_full(pulse))
  {
    delta_micro = time_diff_micro(&pulse->events[0].time, &pulse->events[1].time);
    //Dynamically adjust the glitch period
    if(delta_micro < Glitch_Period)
    {
      Glitch_Period = delta_micro;
    }
    //Reset glitch period if we are at 30 BPM -> period of 1 SEC
    if(delta_micro > (800 * 1000))
    {
      Glitch_Period = GLITCH_PERIOD_30;
    }

    if(within_range(delta_micro, Glitch_Period, GLITCH_RANGE))
    {
      gpio_set_level(GLITCH_PIN, GPIO_LEVEL_LOW);
      clear_pulse(pulse);
      return;
    }

    //Try to join -> Get DELTA
    if(signal->curr_pulse > 0)
    {
      delta_micro = time_diff_micro(&signal->pulses[0].events[1].time, &signal->pulses[1].events[0].time);
      if (delta_micro >= MIN_PERIOD_SQUARE)
      {
        //To far apart -> validate leading then shift
        validate_pulse_square(signal, &signal->pulses[0]); 
        signal->pulses[0] = signal->pulses[1];
        clear_pulse(&signal->pulses[1]);
        signal->curr_pulse = 0;
        return;
      }
      //Join pulses
      //Set latest event into last event of pulse 0.
      printf("JOIN");
      gpio_set_level(JOIN_PIN, GPIO_LEVEL_LOW);
      signal->pulses[0].events[1] = signal->pulses[1].events[1];
      clear_pulse(&signal->pulses[1]);
      return;
    }
    //No leading pulse: Continue along
    signal->curr_pulse++;
    if(signal->curr_pulse >= PULSE_COUNT) {signal->curr_pulse = PULSE_COUNT - 1;}
  }
  return;
}

void process_input(Signal *signal)
{
  struct timeval t_curr = {0, 0};
  gettimeofday(&t_curr, NULL);
  

  //ECG MODE
  if(Cradle_Mode == ECG || Cradle_Mode == ECG_VARIATION)
  {
    process_ecg(t_curr, signal);
  }

  //SQUARE MODE
  if(Cradle_Mode == SQUARE || Cradle_Mode == SQUARE_VARIATION)
  {
    process_square(t_curr, signal);
  }

  fflush(NULL);
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
    int mode = atomic_load(&cradle_atom);
    double frequency;
    int bpm;

    while(atomic_load(&output_kalive))
    {
      double new_period = atomic_load(&period_atom);
      if(new_period == period && atomic_load(&cradle_atom) == mode)
      {
        nanosleep(&t_sleep, NULL); //sleep in microseconds
        continue;
      }
      mode = atomic_load(&cradle_atom);
      period = new_period;
      if(period == 0)
      {
        frequency = 0;
        bpm = 0;
      } else 
      {
        frequency = (500.0) / period;
        bpm = 60 * frequency; //150ms -> 200  // 1000ms -> 30  //500ms -> 60
      }

      printf("---------PERIOD: %f ---------\n", period);
      ui_rprintf(ui, 3, "%qMode: %q%s", RGB_ORANGE, RGB_RED, Cradle_Modes_Str[Cradle_Mode]);
      ui_rprintf(ui, 6, "%qFrequency: %q%f", RGB_PURPLE, RGB_GREEN, frequency);
      ui_rprintf(ui, 7, "%qBPM:[60-240]: %q%d", RGB_PURPLE, RGB_GREEN, bpm);

      ui_rprintf(ui, 9, "%qGlitch Period: %q%d", RGB_YELLOW, RGB_RED, Glitch_Period);


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
  gpio_set_direction(GLITCH_PIN, GPIO_DIR_OUTPUT);
  gpio_set_direction(JOIN_PIN, GPIO_DIR_OUTPUT);

  //ADC init
  adc_init();

  //---Signal struct---
  Signal signal;
  signal.state = 0;
  signal.curr_pulse = 0;
  signal.period = 0;
  signal.idx_reading = 0;
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
  atomic_init(&period_atom, Cradle_Mode);
  atomic_init(&output_kalive, true);
  atomic_init(&cradle_atom, 0);
  signal.period = atomic_load(&period_atom);
  struct thread_arg thrd_args = {&ui, &com};
  pthread_t output_thread;
  pthread_create(&output_thread, NULL, &fnc_output_thread, &thrd_args);

  int button_states[] = {0, 0};
  int bstate;

  //PLOTTING DEBUG
  gettimeofday(&start, NULL);
  Cradle_Mode = ECG;

  while (get_switch_state(1))
  { 
    bstate = get_button_state(0);
    if(bstate != button_states[0])
    {
      Cradle_Mode-= bstate;
      button_states[0] = bstate;
    }
    bstate = get_button_state(1);
    if(bstate != button_states[1])
    {
      Cradle_Mode+= bstate;
      button_states[1] = bstate;
    }

    if(Cradle_Mode < 0) { Cradle_Mode = 0;}
    if(Cradle_Mode > 3) {Cradle_Mode = 3;}
    atomic_store(&cradle_atom, Cradle_Mode);

    process_input(&signal);

    //Mode depended pulse handeling
    if(Cradle_Mode == SQUARE || Cradle_Mode == ECG)
    {
      if(signal.idx_reading >= MIN_READINGS)
      {
        signal.idx_reading = 0;
        sort_readings(signal.readings, MIN_READINGS);
        signal.period = signal.readings[MIN_READINGS / 2] / 1000.0;
        atomic_store(&period_atom, signal.period);
      }
      continue;
    }

    //Try
    /*
      Mean of uneven []
      Average []
      Smallest distance -> isn't that just mean?? []
      f(mean, average) []
      Or maybe pick at random, no clue, statistics was 5 years ago and I am a melon. []
    */
    if(Cradle_Mode == SQUARE_VARIATION || Cradle_Mode == ECG_VARIATION)
    {
      //Wait for a full dataset
      if(signal.idx_reading >= MIN_READINGS_VAR)
      {
      
        //Sort
        sort_readings(signal.readings, MIN_READINGS_VAR);

        //Take the mean -> center index of uneven array
        signal.period = signal.readings[0];
        printf("---------PERIOD: %d ---------\n", signal.period);
        fflush(NULL);
        atomic_store(&period_atom, signal.period);
      }
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