/*
Copyright (c) 2023 Eindhoven University of Technology

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#include <adc.h>
#include <arm_shared_memory_system.h>
#include <errno.h>
#include <log.h>
#include <platform.h>
#include <stdio.h>
#include <stdlib.h>

static struct arm_shared_t adc_handle;
static volatile uint32_t *adc = NULL;

static const uint32_t twopow16 = 0b10000000000000000;

bool invalid_channel_adc(const adc_channel_t channel) {
  if (channel == ADC0) {
    return false;
  }
  if (channel == ADC1) {
    return false;
  }
  if (channel == ADC2) {
    return false;
  }
  if (channel == ADC3) {
    return false;
  }
  if (channel == ADC4) {
    return false;
  }
  if (channel == ADC5) {
    return false;
  }
  return true;
}

bool initialized_adc(void) {
  if (adc == NULL) {
    return false;
  }
  return true;
}

bool check_initialized_adc(void) {
  if (!initialized_adc()) {
    pynq_error("The ADC has not been initialized\n");
  }
  return true;
}

bool check_channel_adc(const adc_channel_t channel) {
  if (invalid_channel_adc(channel)) {
    pynq_error("Invalid ADC channel %d\n", channel);
  }
  return true;
}

void adc_init(void) { adc = arm_shared_init(&adc_handle, xadc_wiz_0, 4096); }

void adc_destroy(void) {
  if (adc != NULL) {
    (void)arm_shared_close(&adc_handle);
    adc = NULL;
  }
}

double adc_read_channel(const adc_channel_t channel) {
  (void)check_channel_adc(channel);
  (void)check_initialized_adc();

  // TODO we need to calibrate this
  double value = adc[channel] * (3.23 / twopow16);

  return value;
}

uint32_t adc_read_channel_raw(adc_channel_t channel) {
  (void)check_channel_adc(channel);
  (void)check_initialized_adc();

  if (adc == NULL) {
    return UINT32_MAX;
  }
  uint32_t value = adc[channel];

  return value;
}
