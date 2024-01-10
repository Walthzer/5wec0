 /*
 *  TU/e 5WEC0::PYNQ RYB System Module Integration. Heartbeat module header
 * 
 *  Written by: Walthzer
 * 
 */

#include <libpynq.h>
#include <stdlib.h>
#include <sys/time.h>
#include <libui.h>
#include <libcom.h>

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
#define ECG_MEASUREMENT_BUFF 32
#define ECG_GLITCH_MOV_WINDOW 4

//Times are in microseconds
#define MAX_PERIOD_SQUARE (1050 * 1000) //Maximum width of a SQUARE period
#define MIN_PERIOD_SQUARE (100 * 1000) //Minium width of a SQUARE period
#define GLITCH_PERIOD_30 (12.5 * 1000)
#define GLITCH_RANGE 500 //Bounds of the glitch period range. 

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