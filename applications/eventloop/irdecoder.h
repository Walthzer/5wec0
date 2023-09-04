#pragma once

enum Order {
  IDLE = 0,
  BITS = 1,
};
typedef struct {
  GTimer *timer;
  pin_t pin;
  uint32_t value;
  uint32_t nbits;
  gpio_direction_t dir;
  enum Order state;
  gpio_level_t prev_level;
} IRDecoder;

gboolean ir_decoder_loop(IRDecoder *o, uint32_t *value);
IRDecoder *ir_decoder_create(pin_t pin);
void ir_decoder_destroy(IRDecoder *o);
void ir_decoder_interrupt_handler(IRDecoder *o);
