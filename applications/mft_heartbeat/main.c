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
#include <math.h>
#include <limits.h>

#define DEBUG

#define MIN(a,b) a > b ? b : a
#define MAX(a,b) a > b ? a : b

#define SENSOR_PIN IO_A0
#define TEST_PIN IO_AR7
#define GLITCH_PIN IO_AR8
#define JOIN_PIN IO_AR9

#define PULSE_COUNT 2
#define MAX_BPM_WINDOW 32

//Sampeling 
#define CD_PERIOD_MICRO 2000
#define ECG_BUFFER_SIZE 128
#define ECG_MEASUREMENT_BUFF 32

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

typedef struct _conditioning_
{
  int median_window[MAX_BPM_WINDOW];
  int c_median;
  int median;
} Filter;

typedef struct _signal_
{
  Filter filter;
  Pulse pulses[PULSE_COUNT]; //Falling Edge timestamps
  int state, curr_pulse;
  int period;
} Signal;

typedef struct _ecg_data
{
  Filter filter;
  struct timeval last;
  int threshold, state, c_event;
  int c_sample;
  unsigned long sum_samples;
  Pulse pulse;
} Ecg_signal;

//Globals
//FLAGS
char *Cradle_Modes_Str[] = {"Square     ", "ECG        "};
enum CRADLE_MODES {SQUARE, ECG};
static int Cradle_Mode = 0;

//PERIOD
struct timeval start;
int Glitch_Period = GLITCH_PERIOD_30;
atomic_int cradle_atom;
atomic_int bpm_atom;
atomic_bool output_kalive;

//Get the truth value of the sensor
bool get_sensor_state()
{
  int state = adc_read_channel_raw(ADC0);
  //printf("State: %f\n", state);
  return state > 10000;
}

//lil sorter
int int_compare(const void* numA, const void* numB)
{
    const int* num1 = (const int*)numA;
    const int* num2 = (const int*)numB;

    if (*num1 > *num2) {
        return 1;
    }
    else {
        if (*num1 == *num2)
            return 0;
        else
            return -1;
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
    return; //TODO 
  }
}


//FILTERING
void filter_clear(Filter* filter)
{
  memset(filter, 0, sizeof(Filter));
}
int filter_median(Filter* filter)
{
  qsort(filter->median_window, filter->c_median+1, sizeof(int), int_compare);
  return filter->median_window[filter->c_median / 2];
}
void filter_resolve(Filter* filter)
{
  //Calculate new filter median and compare to current median
  //Clear the filter and only if we had equal medians -> update
  int new_median = filter_median(filter);
  if(filter->median == new_median)
  {
    atomic_store(&bpm_atom, new_median);
  }
  filter_clear(filter);
  filter->median = new_median;
}

void filter_push(int bpm, Filter* filter)
{
  static int dyn_win;
  filter->median_window[filter->c_median] = round(bpm / 20) * 20;
  
  //Filtering below 80 BPM is not required
  //Push these values directly
  if(filter->median_window[filter->c_median] <= 80)
  {
    atomic_store(&bpm_atom, filter->median_window[filter->c_median]);
    filter_clear(filter);
  }

  if(filter->c_median == 0)
  {
    //Calculate desired dynamic window based on BPM
    //Higher bpm -> more issues -> more measurements
    //Enforce result as odd number for window centering
    dyn_win = 2*floor(pow(2.0f, (filter->median_window[0] / 50.0))/2.0);
    dyn_win = MIN(dyn_win,13);
  }

  printf("[");
  for (int i = 0; i < MAX_BPM_WINDOW; i++)
  {
    printf("%3d ", filter->median_window[i]);
  }
  printf("]\n Median: %d -- BPM: %d\n", filter->median, atomic_load(&bpm_atom));


  printf("C: %d --- Dyn: %d\n", filter->c_median, dyn_win);
  //Clear the filter and process data when:
  //  * Filter is full
  //  * We risk an overflow
  if ( !(filter->c_median >= dyn_win || filter->c_median >= MAX_BPM_WINDOW-1) )
  {
    filter->c_median++;
    return;
  }
  //Process the filter
  filter_resolve(filter);
  
}

void process_ecg(struct timeval t_curr, Ecg_signal *signal)
{
  static int bpm, sample, c_event;
  static struct timeval events[2] = {{0, 0}, {0, 0}};

  if(time_diff_micro(&t_curr, &signal->last) >= CD_PERIOD_MICRO)
  {
    signal->last = t_curr;
    //x = time_diff_micro(&t_curr, &start);
    sample = adc_read_channel_raw(ADC0);
    signal->sum_samples += sample;
    signal->c_sample++;
    //printf("%d,%d\n", x, signal->s_buff[signal->idx_sample]); //Output CSV
  } else {return;};
  
  //Insufficient samples
  if(signal->c_sample < ECG_MEASUREMENT_BUFF) {return;}
  
  //Prevent Overflows of the sum_samples
  //Probably terriable but eh.
  if(signal->sum_samples > INT_MAX)
  {
    signal->sum_samples = signal->sum_samples / 2;
    signal->c_sample = signal->c_sample / 2;
  }

  //calculate the threshold
  signal->threshold = (signal->sum_samples / signal->c_sample) + 3e4;
  //printf("Threshold: %d  Sample: %d \n", signal->threshold, sample);
  if (sample > signal->threshold && signal->state == 0)
  {
    signal->state = 1;
    events[c_event] = t_curr;
    gpio_set_level(TEST_PIN, GPIO_LEVEL_HIGH);

    if (c_event >= 1)
    {
        //calculate period
        c_event = 1;
        bpm = round(60.0 * (1000000.0 / (double)time_diff_micro(&events[0], &events[1]))) + 20;

        //x = time_diff_micro(&t_curr, &start);
        //printf("%f,%d,%d,%d\n", bpm, x, sample, signal->threshold); //Output CSV
        //fflush(NULL);

        filter_push(bpm, &signal->filter);

        //swap
        events[0] = events[1];
    }
    else
    {
      c_event = 1;
    }
  }
  if (sample < signal->threshold && signal->state == 1)
  {
    signal->state = 0;
    gpio_set_level(TEST_PIN, GPIO_LEVEL_LOW);
  }
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

void* fnc_output_thread(void* arg)
{
    struct thread_arg* thrd_args = arg;
    ui_t* ui = thrd_args->ui;
    com_t* com = thrd_args->com;

    //outputs
    //com_put(&com, HEARTBEAT, data.bpm);
    //com_get(&com, HEARTBEAT, &data.bpm);
    
    struct timespec t_sleep = {0, 10000};
    int mode = atomic_load(&cradle_atom);
    int bpm;

    while(atomic_load(&output_kalive))
    {
      int new_bpm = atomic_load(&bpm_atom);
      if(new_bpm == bpm && atomic_load(&cradle_atom) == mode)
      {
        nanosleep(&t_sleep, NULL); //sleep in microseconds
        continue;
      }
      mode = atomic_load(&cradle_atom);
      bpm = new_bpm;

      ui_rprintf(ui, 3, "%qMode: %q%s", RGB_ORANGE, RGB_RED, Cradle_Modes_Str[Cradle_Mode]);
      ui_rprintf(ui, 6, "%qBPM:[60-240]: %q%d", RGB_PURPLE, RGB_GREEN, bpm);

      ui_rprintf(ui, 8, "%qGlitch Period: %q%d", RGB_YELLOW, RGB_RED, Glitch_Period);


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
  Signal signal = {};
  //---END Signal---
  //---ECG struct---

  Ecg_signal ecg_signal = {};

  //---END ECG----
  //---END Vars---

  //Threading
  atomic_init(&bpm_atom, 0);
  atomic_init(&output_kalive, true);
  atomic_init(&cradle_atom, 0);
  signal.period = atomic_load(&bpm_atom);
  struct thread_arg thrd_args = {&ui, &com};
  pthread_t output_thread;
  pthread_create(&output_thread, NULL, &fnc_output_thread, &thrd_args);

  int button_states[] = {0, 0};
  int bstate;

  //PLOTTING DEBUG
  gettimeofday(&start, NULL);
  Cradle_Mode = ECG;

  //TIME KEEPING
  struct timeval t_curr = {0, 0};

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

    gettimeofday(&t_curr, NULL);
    

    //ECG MODE
    if(Cradle_Mode == ECG)
    {
      process_ecg(t_curr, &ecg_signal);
    }

    //SQUARE MODE
    if(Cradle_Mode == SQUARE)
    {
      process_square(t_curr, &signal);
    }
    fflush(NULL);

    //Run pulses through the clamp filtering

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