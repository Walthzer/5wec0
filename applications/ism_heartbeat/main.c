 /*
 *  TU/e 5WEC0::PYNQ RYB System Module Integration. Heartbeat module
 *
 *  Interface:
 *  Switch 1 -> Exits the main loop
 *  Switch 0 -> Toggle Filter Skip
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

#include "heartbeat.h"

#undef LOG_DOMAIN
#define LOG_DOMAIN "mft_heartbeat"

//Globals
//FLAGS
char *Cradle_Modes_Str[] = {"Square     ", "Sqr Glitch", "ECG        ", "ECG Glitch"};
enum CRADLE_MODES {SQUARE, SQUARE_GLITCH, ECG, ECG_GLITCH};
static int Cradle_Mode = 0;

//PERIOD
struct timeval start;
int Glitch_Period = GLITCH_PERIOD_30;
atomic_int cradle_atom;
atomic_int bpm_atom;
atomic_bool output_kalive;

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
  bpm = round(bpm / 20.0) * 20;
  filter->median_window[filter->c_median] = bpm;
  
  //Filtering below 80 BPM is not required
  //Push these values directly
  if(bpm <= 80 && get_switch_state(0))
  {
    atomic_store(&bpm_atom, bpm);
    filter_clear(filter);
    filter->median = bpm;
  }

  if(filter->c_median == 0)
  {
    //Calculate desired dynamic window based on BPM
    //Higher bpm -> more issues -> more measurements
    //Enforce result as odd number for window centering
    dyn_win = 2*floor(pow(2.0f, (filter->median_window[0] / 50.0))/2.0);
    dyn_win = MIN(dyn_win,13);
  }

  if(Cradle_Mode == ECG)
  {
    dyn_win = 0;
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

void validate_pulse_square(Signal *signal, Pulse *pulse)
{
  if(!pulse_full(pulse))
  {
    pynq_error("Trying to validate a non-full pulse??");
    return;
  }

  int delta_micro = time_diff_micro(&pulse->events[0].time, &pulse->events[1].time);
  clear_pulse(pulse);
  //Prevent Strange periods from leaking through
  if(delta_micro < MAX_PERIOD_SQUARE && delta_micro > MIN_PERIOD_SQUARE)
  {
    int bpm = round(60.0 * (500000.0 / (double)delta_micro));
    filter_push(bpm, &signal->filter);
  }
}

void process_ecg(struct timeval t_curr, Ecg_signal *signal)
{
  static int bpm, sample, c_event;
  static int movmean[ECG_GLITCH_MOV_WINDOW], p_mov;
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
    printf("OVERFLOW\n");
    signal->sum_samples = signal->sum_samples / 2;
    signal->c_sample = signal->c_sample / 2;
  }

  //Split threshold calcuation and add filtering for ECG-Glitch
  if(Cradle_Mode == ECG_GLITCH)
  {
    int mean = (signal->sum_samples / signal->c_sample);
    int cuttoff = mean + 13e3;
    signal->threshold =  mean + 8e3;

    //Focus on the T wave
    //-> Discard peaks and valley's
    if (sample > cuttoff || sample < 3e4) { return; }

    //Take the moving mean -> window size ECG_GLITCH_MOV_WINDOW
    if(p_mov >= ECG_GLITCH_MOV_WINDOW)
    {
      //Take total and shift buffer
      int mean_val = 0;
      for(int i = 0; i < ECG_GLITCH_MOV_WINDOW-1; i++)
      {
          mean_val += movmean[i+1];
          movmean[i] = movmean[i+1];
      }
      movmean[ECG_GLITCH_MOV_WINDOW-1] = sample;
      mean_val += sample;

      //Take the mean as sample value
      sample = mean_val / ECG_GLITCH_MOV_WINDOW;

    } else {
      //Fill buffer first
      movmean[p_mov] = sample;
      p_mov++;
      return;
    }

  } else {
    //Reset the GLITCH variables
    p_mov = 0;

    signal->threshold = (signal->sum_samples / signal->c_sample) + 3e4;
  }

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
        bpm = round(60.0 * (1000000.0 / (double)time_diff_micro(&events[0], &events[1])));

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
    pulse->events[0].type = event;
    pulse->events[0].time = t_curr;

    gpio_set_level(TEST_PIN, GPIO_LEVEL_HIGH);
    gpio_set_level(GLITCH_PIN, GPIO_LEVEL_HIGH);
    gpio_set_level(JOIN_PIN, GPIO_LEVEL_HIGH);
  }

  if(event == FALL)
  {
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

void* fnc_ui_thread (void* arg)
{
    struct thread_arg* thrd_args = arg;
    ui_t* ui = thrd_args->ui;

    //outputs
    struct timespec t_sleep = {0, 10000};
    int mode = -1;
    int low_skip = -1;
    uint32_t bpm;

    while(atomic_load(&output_kalive))
    {
      uint32_t new_bpm = atomic_load(&bpm_atom);
      if(new_bpm == bpm && atomic_load(&cradle_atom) == mode && low_skip == get_switch_state(0))
      {
        nanosleep(&t_sleep, NULL); //sleep in microseconds
        continue;
      }
      low_skip = get_switch_state(0);
      mode = atomic_load(&cradle_atom);
      bpm = new_bpm;

      if(bpm < 60) {bpm = 60;}
      if(bpm > 240) {bpm = 240;}

      ui_rprintf(ui, 3, "%qMode: %q%s", RGB_ORANGE, RGB_RED, Cradle_Modes_Str[Cradle_Mode]);
      ui_rprintf(ui, 4, "%qFilter Skip: %q%s", RGB_ORANGE, get_switch_state(0) ? RGB_GREEN : RGB_RED, get_switch_state(0) ? "80" : "None");
      ui_rprintf(ui, 6, "%qGlitch Period: %q%d", RGB_YELLOW, RGB_RED, Glitch_Period);
      ui_rprintf(ui, 9, "%qBPM:[60-240]: %q%d", RGB_PURPLE, RGB_GREEN, bpm);


      ui_draw(ui);
    }
    return NULL;
}

void* fnc_com_thread (void* arg)
{
    struct thread_arg* thrd_args = arg;
    com_t* com = thrd_args->com;

    //outputs
    uint32_t bpm;

    while(atomic_load(&output_kalive))
    {
      bpm = atomic_load(&bpm_atom);

      if(bpm < 60) {bpm = 60;}
      if(bpm > 240) {bpm = 240;}

      com_put(com, HEARTBEAT, bpm);
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
  ui_rprintf(&ui, 4, "%qAdd 20: %q%s", RGB_ORANGE, RGB_RED, "No");
  //Internal: Glitch-period
  ui_rprintf(&ui, 5, "%q--Internal--", RGB_CYAN);
  ui_rcenter(&ui, 5, true);
  //Outputs: Heartbeat
  ui_rprintf(&ui, 8, "%q--Outputs--", RGB_CYAN);
  ui_rcenter(&ui, 8, true);

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
  pthread_t ui_thread;
  pthread_t com_thread;
  pthread_create(&ui_thread , NULL, &fnc_ui_thread , &thrd_args);
  pthread_create(&com_thread , NULL, &fnc_com_thread , &thrd_args);


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
    if(Cradle_Mode == ECG || Cradle_Mode == ECG_GLITCH)
    {
      process_ecg(t_curr, &ecg_signal);
    }

    //SQUARE MODE
    if(Cradle_Mode == SQUARE || Cradle_Mode == SQUARE_GLITCH)
    {
      process_square(t_curr, &signal);
    }
    fflush(NULL);

    //Run pulses through the clamp filtering

  }

  //threading 
  //gracefull exit from thread;
  atomic_store(&output_kalive, false);
  pthread_join(ui_thread , NULL);
  pthread_join(com_thread , NULL);

  //ADC
  adc_destroy();

  buttons_destroy();
  switches_destroy();
  com_destroy(&com);
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};