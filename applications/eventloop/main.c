#include <glib-unix.h>
#include <glib.h>
#include <libpynq.h>
#include <stdio.h>

#include "irdecoder.h"

IRDecoder *IRDecode = NULL;
gboolean interrupt_callback(gint fd, GIOCondition condition,
                            gpointer user_data __attribute__((unused))) {

  int m = 1;
  if (condition == G_IO_IN) {
    int d = read(fd, &m, sizeof(int));
    if (d > 0) {
      ir_decoder_interrupt_handler(IRDecode);
      gpio_ack_interrupt();
      m = 1;
      write(fd, &m, sizeof(m));
    }
  }
  return TRUE;
}

gboolean setup(void) {
  pynq_init();
  int fd = gpio_interrupt_init();
  g_unix_fd_add(fd, G_IO_IN, interrupt_callback, NULL);

  IRDecode = ir_decoder_create(SWB_AR7);

  return TRUE;
}

void loop(void) {
  uint32_t msg = 0;
  if ((ir_decoder_loop(IRDecode, &msg))) {
    printf("Got message: %d\r\n", msg);
  }
}
void destroy(void) {

  ir_decoder_destroy(IRDecode);
  pynq_destroy();
  switchbox_destroy();
}
