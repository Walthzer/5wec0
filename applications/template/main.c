#include <libpynq.h>

int main(void) {
  pynq_init();

  // your code here
  printf("hello, world!");

  pynq_destroy();
  return EXIT_SUCCESS;
}
