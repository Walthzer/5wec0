#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include <libpynq.h>

int main(int argc __attribute__((unused)),
         char **argv __attribute__((unused))) {
  // Setting up the configuration for reading analog pins
  pynq_init();
  adc_init();
  gpio_set_direction(SWB_A0, GPIO_DIR_OUTPUT);
  gpio_set_direction(SWB_A1, GPIO_DIR_OUTPUT);
  gpio_set_direction(SWB_A2, GPIO_DIR_OUTPUT);
  gpio_set_direction(SWB_A3, GPIO_DIR_OUTPUT);
  gpio_set_direction(SWB_A4, GPIO_DIR_OUTPUT);
  gpio_set_direction(SWB_A5, GPIO_DIR_OUTPUT);

  // Reading all analog pins after first setting their output-value
  // First setting LOW, reading, setting output HIGH, reading again
  gpio_set_level(SWB_A0, GPIO_LEVEL_LOW);
  sleep_msec(10);
  double vl = adc_read_channel(ADC0);
  uint32_t vlr = adc_read_channel_raw(ADC0);
  gpio_set_level(SWB_A0, GPIO_LEVEL_HIGH);
  sleep_msec(10);
  double vh = adc_read_channel(ADC0);
  uint32_t vhr = adc_read_channel_raw(ADC0);
  printf("Value A0: %f-%f %04X %04X\n", vl, vh, vlr, vhr);
  gpio_set_level(SWB_A0, GPIO_LEVEL_LOW);

  gpio_set_level(SWB_A1, GPIO_LEVEL_LOW);
  sleep_msec(10);
  vl = adc_read_channel(ADC1);
  gpio_set_level(SWB_A1, GPIO_LEVEL_HIGH);
  sleep_msec(10);
  vh = adc_read_channel(ADC1);
  printf("Value A1: %f-%f\n", vl, vh);
  gpio_set_level(SWB_A1, GPIO_LEVEL_LOW);

  gpio_set_level(SWB_A2, GPIO_LEVEL_LOW);
  sleep_msec(10);
  vl = adc_read_channel(ADC2);
  gpio_set_level(SWB_A2, GPIO_LEVEL_HIGH);
  sleep_msec(10);
  vh = adc_read_channel(ADC2);
  printf("Value A2: %f-%f\n", vl, vh);
  gpio_set_level(SWB_A2, GPIO_LEVEL_LOW);

  gpio_set_level(SWB_A3, GPIO_LEVEL_LOW);
  sleep_msec(10);
  vl = adc_read_channel(ADC3);
  gpio_set_level(SWB_A3, GPIO_LEVEL_HIGH);
  sleep_msec(10);
  vh = adc_read_channel(ADC3);
  printf("Value A3: %f-%f\n", vl, vh);
  gpio_set_level(SWB_A3, GPIO_LEVEL_LOW);

  gpio_set_level(SWB_A4, GPIO_LEVEL_LOW);
  sleep_msec(10);
  vl = adc_read_channel(ADC4);
  gpio_set_level(SWB_A4, GPIO_LEVEL_HIGH);
  sleep_msec(10);
  vh = adc_read_channel(ADC4);
  printf("Value A4: %f-%f\n", vl, vh);
  gpio_set_level(SWB_A4, GPIO_LEVEL_LOW);

  gpio_set_level(SWB_A5, GPIO_LEVEL_LOW);
  sleep_msec(10);
  vl = adc_read_channel(ADC5);
  gpio_set_level(SWB_A5, GPIO_LEVEL_HIGH);
  sleep_msec(10);
  vh = adc_read_channel(ADC5);
  printf("Value A5: %f-%f\n", vl, vh);
  gpio_set_level(SWB_A5, GPIO_LEVEL_LOW);

  // Setting the pins to their default setting and clearing the switchbox
  gpio_set_direction(SWB_A0, GPIO_DIR_INPUT);
  gpio_set_direction(SWB_A1, GPIO_DIR_INPUT);
  gpio_set_direction(SWB_A2, GPIO_DIR_INPUT);
  gpio_set_direction(SWB_A3, GPIO_DIR_INPUT);
  gpio_set_direction(SWB_A4, GPIO_DIR_INPUT);
  gpio_set_direction(SWB_A5, GPIO_DIR_INPUT);
  adc_destroy();
  pynq_destroy();
  return EXIT_SUCCESS;
}
