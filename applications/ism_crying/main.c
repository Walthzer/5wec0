 /*
 *  TU/e 5WEC0::PYNQ RYB Module Testing. Crying module
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

#include "crying.h"

#define DEBUG

//Sampeling 
#define CD_PERIOD_MICRO 500
#define WINDOW_SIZE 2000
#define THRESHOLD 3e4

#undef LOG_DOMAIN
#define LOG_DOMAIN "ism_crying"

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
  //Internal: N.V.T.
  ui_rprintf(&ui, 2, "%q--Internal--", RGB_CYAN);
  ui_rcenter(&ui, 2, true);
  //Outputs: Crying
  ui_rprintf(&ui, 5, "%q--Outputs--", RGB_CYAN);
  ui_rcenter(&ui, 5, true);

  ui_draw(&ui);

  com_t com;
  com_init(&com, CRYING);

  //Init inputs
  gpio_init();
  switches_init();
  buttons_init();

  //ADC init
  adc_init();

  //Filter setup
  Filter filter;
  filter_clear(&filter);

  //Threading
  atomic_init(&cry_atom, 100);
  atomic_init(&output_kalive, true);
  pthread_t ui_thread;
  pthread_t com_thread;
  pthread_create(&ui_thread , NULL, &fnc_ui_thread , &ui);
  pthread_create(&com_thread , NULL, &fnc_com_thread , &com);

  //TIME KEEPING
  struct timeval t_epoch, t_now, t_start;
  gettimeofday(&t_start, NULL);
  t_epoch = t_now = t_start;

  unsigned long int sum_samples;
  int c_sample;
  double sample, mean;
  fprintf(stderr, "Running....\n");
  while (get_switch_state(1))
  { 
    gettimeofday(&t_now, NULL);
    if(time_diff_micro(&t_start, &t_now) >= CD_PERIOD_MICRO)
    {
      t_start = t_now;
      sample = adc_read_channel_raw(ADC0);
      //Add good samples, discard bad ones
      if(sample > THRESHOLD)
        sum_samples = sum_samples + 10;

      c_sample++;
      //printf("%d,%d\n", x, signal->s_buff[signal->idx_sample]); //Output CSV
    } else {continue;};
    
    //Insufficient samples
    if(c_sample < WINDOW_SIZE) { continue; }
    
    //We have enough samples, calculate the mean and cleanup
    mean = (sum_samples / (double)c_sample);
    sum_samples = c_sample = 0.0;

    //printf("Threshold: %d  Sample: %d \n", signal->threshold, sample);
    //fprintf(stderr, "Mean: %f --- Percent: %d\n", mean, to_percent(mean));
    filter_push(mean, &filter);
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