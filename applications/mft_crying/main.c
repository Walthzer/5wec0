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

#define DEBUG

#define MIN(a,b) a > b ? b : a
#define MAX(a,b) a > b ? a : b

#define SENSOR_PIN IO_A0
#define TEST_PIN IO_AR7
#define GLITCH_PIN IO_AR8
#define JOIN_PIN IO_AR9

//Sampeling 
#define CD_PERIOD_MICRO 500

#undef LOG_DOMAIN
#define LOG_DOMAIN "mft_crying"

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

  //TIME KEEPING
  struct timeval t_epoch, t_now, t_start;
  gettimeofday(&t_start, NULL);
  t_epoch = t_now = t_start;

  int sample;
  while (get_switch_state(1))
  { 
    
    if(time_diff_micro(&t_start, &t_now) >= CD_PERIOD_MICRO)
    {
      t_start = t_now;
      int x = time_diff_micro(&t_now, &t_epoch);
      int sample = adc_read_channel_raw(ADC0);
      printf("%d,%d\n", x, sample); //Output CSV
    };
    gettimeofday(&t_now, NULL);
  }

  //ADC
  adc_destroy();

  buttons_destroy();
  switches_destroy();
  com_destroy(&com);
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};