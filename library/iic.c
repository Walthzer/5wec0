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
#include "iic.h"
#include "arm_shared_memory_system.h"
#include "log.h"
#include "xiic_l.h"
#include <platform.h>
#include <stdio.h>
#include <string.h>

static arm_shared iic_handles[NUM_IICS];
static volatile uint32_t *iic_ptrs[NUM_IICS] = {
    NULL,
};

#define IIC_REG_SOFT_RESET (0x40 / 4)

void iic_init(const iic_index_t iic) {
  if (!(iic >= IIC0 && iic < NUM_IICS)) {
    pynq_error("invalid IIC %d, must be 0..%d\n", iic, NUM_IICS);
  }
  if (iic == IIC0) {
    iic_ptrs[iic] = arm_shared_init(&(iic_handles[iic]), axi_iic_0, 4096);
  } else if (iic == IIC1) {
    iic_ptrs[iic] = arm_shared_init(&(iic_handles[iic]), axi_iic_1, 4096);
  }
  // Reset
  iic_ptrs[iic][IIC_REG_SOFT_RESET] = 0xA;
}

void iic_destroy(const iic_index_t iic) {
  if (!(iic >= IIC0 && iic < NUM_IICS)) {
    pynq_error("invalid IIC %d, must be 0..%d-1\n", iic, NUM_IICS);
  }
  arm_shared_close(&(iic_handles[iic]));
  iic_ptrs[iic] = NULL;
}

bool iic_read_register(const iic_index_t iic, const uint8_t addr,
                       const uint8_t reg, uint8_t *data, uint16_t data_length) {
  if (!(iic >= IIC0 && iic < NUM_IICS)) {
    pynq_error("invalid IIC %d, must be 0..%d-1\n", iic, NUM_IICS);
  }
  XIic_Send((UINTPTR)iic_ptrs[iic], addr, (u8 *)&reg, 1, XIIC_REPEATED_START);
  uint8_t ByteCount =
      XIic_Recv((UINTPTR)iic_ptrs[iic], addr, data, data_length, XIIC_STOP);
  return (ByteCount == data_length) ? 0 : 1;
}

bool iic_write_register(const iic_index_t iic, const uint8_t addr,
                        const uint8_t reg, uint8_t *data,
                        uint16_t data_length) {
  if (!(iic >= IIC0 && iic < NUM_IICS)) {
    pynq_error("invalid IIC %d, must be 0..%d-1\n", iic, NUM_IICS);
  }
  uint8_t buffer[1 + data_length];
  buffer[0] = reg;
  memcpy(&(buffer[1]), data, data_length);
  uint8_t ByteCount = XIic_Send((UINTPTR)iic_ptrs[iic], addr, &(buffer[0]),
                                1 + data_length, XIIC_STOP);
  return (ByteCount == (data_length + 1)) ? 0 : 1;
}
