#include <libpynq.h>
#include <time.h>

static inline void timespec_diff(struct timespec *a, struct timespec *b,
                                 struct timespec *result) {
  result->tv_sec = a->tv_sec - b->tv_sec;
  result->tv_nsec = a->tv_nsec - b->tv_nsec;
  if (result->tv_nsec < 0) {
    --result->tv_sec;
    result->tv_nsec += 1000000000L;
  }
}

int main(void) {
  // Initialize devices
  pynq_init();

  // Initialize interrupts and output
  gpio_interrupt_init();
  gpio_set_direction(SWB_LD0, GPIO_DIR_OUTPUT);
  switchbox_set_pin(SWB_LD4R, SWB_PWM0);
  gpio_disable_all_interrupts();
  gpio_ack_interrupt();
  gpio_print_interrupt();
  gpio_enable_interrupt(SWB_BTN0);
  gpio_enable_interrupt(SWB_BTN1);
  gpio_enable_interrupt(SWB_BTN2);
  gpio_enable_interrupt(SWB_BTN3);
  gpio_print_interrupt();

  printf("wait for interrupt\r\n");
  struct timespec start, end;
  clock_gettime(CLOCK_REALTIME, &start);
  while (1) {
    gpio_wait_for_interrupt(64);
    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec diff;
    timespec_diff(&end, &start, &diff);
    printf("time between interrupts in seconds %lu, in nsec %lu\r\n",
           diff.tv_sec, diff.tv_nsec);
    {
      gpio_print_interrupt();
      uint8_t interruptPin[64] = {0};
      gpio_get_interrupt_pins(interruptPin);
      if (interruptPin[0] == SWB_BTN0) {
        printf("turning on LED as pin button 0 is triggered \n");
        gpio_set_level(SWB_LD0, 1);
      } else {
        gpio_set_level(SWB_LD0, 0);
        printf("interrupt on pin %d\n", interruptPin[0]);
      }
      gpio_ack_interrupt();
    }
    start = end;
  }

  pynq_destroy();
  return EXIT_SUCCESS;
}
