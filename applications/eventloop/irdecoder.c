#include <glib.h>
#include <libpynq.h>

#include "irdecoder.h"

#define MARGIN 0.0002
#define MARKZERO 0.0006
#define MARKONE 0.0012
#define STARTBIT 0.0024

void ir_decoder_interrupt_handler(IRDecoder *o) {
  gpio_level_t l = gpio_get_level(SWB_AR7);
  if (o->prev_level != l) {
    if (l == 0) {
      double t = g_timer_elapsed(o->timer, NULL);
      if (t > 0.003) {
        o->state = IDLE;
      }
      // start of mark.
      g_timer_reset(o->timer);
    } else {
      // end of mark.
      double t = g_timer_elapsed(o->timer, NULL);
      if (o->state == IDLE) {
        if (t > (STARTBIT - MARGIN) && t < (STARTBIT + MARGIN)) {
          o->state = BITS;
          o->value = 0;
          o->nbits = 0;
        }
      } else if (o->state == BITS) {
        if (t > (MARKZERO - MARGIN) && t < (MARKZERO + MARGIN)) {
          // 0
          o->nbits++;
        } else if (t > (MARKONE - MARGIN) && t < (MARKONE + MARGIN)) {
          // 1
          o->nbits++;
          o->value <<= 1;
          o->value |= 1;
        } else {
          // error
          o->state = IDLE;
          o->nbits = 0;
          o->value = 0;
        }
      }
      g_timer_reset(o->timer);
    }
    o->prev_level = l;
  }
}

void ir_decoder_destroy(IRDecoder *o) {
  g_timer_destroy(o->timer);
  g_free(o);
}

gboolean ir_decoder_loop(IRDecoder *o, uint32_t *value) {
  // if timer is longer then one bit will last we know transmission is done.
  if (o->state == BITS) {
    if (g_timer_elapsed(o->timer, NULL) > 0.0014) {
      o->state = IDLE;
      if (o->nbits > 0) {
        if (value != NULL) {
          *value = o->value;
        }
        return TRUE;
      }
    }
  }
  return FALSE;
}

IRDecoder *ir_decoder_create(pin_t pin) {
  IRDecoder *o = (IRDecoder *)g_malloc0(sizeof(IRDecoder));
  o->timer = g_timer_new();
  o->pin = pin;

  gpio_set_direction(o->pin, GPIO_DIR_INPUT);
  gpio_enable_interrupt(o->pin);

  return o;
}
