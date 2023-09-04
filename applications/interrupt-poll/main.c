#include <errno.h>
#include <time.h>

#include <libpynq.h>

static inline void timespec_diff(struct timespec *a, struct timespec *b,
                                 struct timespec *result) {
  result->tv_sec = a->tv_sec - b->tv_sec;
  result->tv_nsec = a->tv_nsec - b->tv_nsec;
  if (result->tv_nsec < 0) {
    --result->tv_sec;
    result->tv_nsec += 1000000000L;
  }
}
int main(int argc __attribute__((unused)),
         char **argv __attribute__((unused))) {
  pynq_init();

  gpio_interrupt_init();
  gpio_disable_all_interrupts();
  gpio_ack_interrupt();
  gpio_print_interrupt();
  gpio_enable_interrupt(SWB_BTN0);
  gpio_enable_interrupt(SWB_BTN1);
  gpio_enable_interrupt(SWB_BTN2);
  gpio_enable_interrupt(SWB_BTN3);
  // gpio_enable_interrupt(SWB_LD4R);
  gpio_print_interrupt();

  printf("wait for interrupt\r\n");
  struct timespec start, end;
  clock_gettime(CLOCK_REALTIME, &start);
  while (1) {

    uint64_t intc = gpio_get_interrupt();
    while (intc == 0) {
      intc = gpio_get_interrupt();
    }
    printf("%016llX\r\n", intc);
    clock_gettime(CLOCK_REALTIME, &end);
    struct timespec diff;
    timespec_diff(&end, &start, &diff);
    printf("%lu %lu\r\n", diff.tv_sec, diff.tv_nsec);
    {
      gpio_print_interrupt();
      gpio_ack_interrupt();
    }
    start = end;
  }

  pynq_destroy();
  return EXIT_SUCCESS;
}
