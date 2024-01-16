 /*
 *  TU/e 5WEC0::PYNQ RYB System Module Integration. Crying module header
 * 
 *  Written by: Walthzer
 * 
 */

#include <libpynq.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>
#include <stdatomic.h>
#include <libui.h>
#include <libcom.h>

#define MIN(a,b) a > b ? b : a
#define MAX(a,b) a > b ? a : b

#define MAX_FILTER_WINDOW 6

typedef struct _conditioning_
{
  int median_window[MAX_FILTER_WINDOW];
  int c_median;
  int median;
} Filter;

atomic_uint_fast32_t cry_atom;
atomic_int output_kalive;

void* fnc_ui_thread (void* arg)
{
    ui_t* ui = arg;
    //outputs
    struct timespec t_sleep = {0, 10000};
    uint32_t cry;
    cry = -1;

    while(atomic_load(&output_kalive))
    {
      if( atomic_load(&cry_atom) == cry )
      {
        nanosleep(&t_sleep, NULL); //sleep in microseconds
        continue;
      }
      cry = atomic_load(&cry_atom);

      ui_rprintf(ui, 6, "%qCrying[0-100]: %q%d", RGB_BLUE, RGB_RED, cry);

      ui_draw(ui);
    }
    return NULL;
}
void* fnc_com_thread (void* arg)
{
    com_t* com = arg;
    //outputs
    uint32_t cry;

    while(atomic_load(&output_kalive))
    {
      cry = 0;
      sched_yield();
    }
    return NULL;
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

//0.2 greater then => 50 percent
//0.6 greater then => 75 percent
//1.2 greater then => 100 percent
uint32_t to_percent(double value)
{
  if (value > 1.2)
    return 100;
  if (value > 0.6)
    return 75;
  if (value > 0.2)
    return 50;
  return 0;
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
  filter_clear(filter);
  atomic_store(&cry_atom, new_median);
  return;

  if(filter->median == new_median)
  {
    return;
  }
  filter_clear(filter);
  filter->median = new_median;
}

void filter_push(float cry, Filter* filter)
{
  static int dyn_win = 5;
  filter->median_window[filter->c_median] = to_percent(cry);

  printf("[");
  for (int i = 0; i < MAX_FILTER_WINDOW; i++)
  {
    printf("%3d ", filter->median_window[i]);
  }
  printf("]\n Median: %d -- BPM: %d\n", filter->median, atomic_load(&cry_atom));


  printf("C: %d --- Dyn: %d\n", filter->c_median, dyn_win);
  //Clear the filter and process data when:
  //  * Filter is full
  //  * We risk an overflow
  if ( !(filter->c_median >= dyn_win || filter->c_median >= MAX_FILTER_WINDOW-1) )
  {
    filter->c_median++;
    return;
  }
  //Process the filter
  filter_resolve(filter);
  
}
