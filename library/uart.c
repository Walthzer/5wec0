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
#include "uart.h"
#include "arm_shared_memory_system.h"
#include "log.h"
#include <platform.h>
#include <stdio.h>

#define UART_REG_RECEIVE_FIFO 0
#define UART_REG_TRANSMIT_FIFO 1
#define UART_REG_STATUS 2
#define UART_REG_CONTROL 3

#define UART_REG_STATUS_BIT_RX_FIFO_HAS_DATA 1
#define UART_REG_STATUS_BIT_RX_FIFO_FULL 2
#define UART_REG_STATUS_BIT_TX_FIFO_EMPTY 4
#define UART_REG_STATUS_BIT_TX_FIFO_FULL 8

#define UART_REG_CONTROL_BIT_CLEAR_TX_FIFO 1
#define UART_REG_CONTROL_BIT_CLEAR_RX_FIFO 2
#define UART_REG_CONTROL_BIT_CLEAR_FIFOS                                       \
  (UART_REG_CONTROL_BIT_CLEAR_RX_FIFO | UART_REG_CONTROL_BIT_CLEAR_TX_FIFO)

static arm_shared uart_handles[NUM_UARTS];
static volatile uint32_t *uart_ptrs[NUM_UARTS] = {
    NULL,
};

void uart_init(const int uart) {
  if (!(uart >= UART0 && uart < NUM_UARTS)) {
    pynq_error("invalid UART %d, must be 0..%d-1\n", uart, NUM_UARTS);
  }
  if (uart == UART0) {
    uart_ptrs[uart] =
        arm_shared_init(&(uart_handles[uart]), axi_uartlite_0, 4096);
  } else if (uart == UART1) {
    uart_ptrs[uart] =
        arm_shared_init(&(uart_handles[uart]), axi_uartlite_1, 4096);
  }
}

void uart_destroy(const int uart) {
  if (!(uart >= UART0 && uart < NUM_UARTS)) {
    pynq_error("invalid UART %d, must be 0..%d-1\n", uart, NUM_UARTS);
  }
  if (uart_ptrs[uart] == NULL) {
    pynq_error("UART%d has not been initialized.\n", uart);
  }
  arm_shared_close(&(uart_handles[uart]));
  uart_ptrs[uart] = NULL;
}

void uart_send(const int uart, const uint8_t data) {
  if (!(uart >= UART0 && uart < NUM_UARTS)) {
    pynq_error("invalid UART %d, must be 0..%d-1\n", uart, NUM_UARTS);
  }
  if (uart_ptrs[uart] == NULL) {
    pynq_error("UART%d has not been initialized.\n", uart);
  }
  while ((uart_ptrs[uart][UART_REG_STATUS] &
          UART_REG_STATUS_BIT_TX_FIFO_FULL) == UART_REG_STATUS_BIT_TX_FIFO_FULL)
    ;
  uart_ptrs[uart][UART_REG_TRANSMIT_FIFO] = data;
}

uint8_t uart_recv(const int uart) {
  if (!(uart >= UART0 && uart < NUM_UARTS)) {
    pynq_error("invalid UART %d, must be 0..%d-1\n", uart, NUM_UARTS);
  }
  if (uart_ptrs[uart] == NULL) {
    pynq_error("UART%d has not been initialized.\n", uart);
  }
  while ((uart_ptrs[uart][UART_REG_STATUS] &
          UART_REG_STATUS_BIT_RX_FIFO_HAS_DATA) == 0) {
  }
  return uart_ptrs[uart][UART_REG_RECEIVE_FIFO];
}

bool uart_has_data(const int uart) {
  if (!(uart >= UART0 && uart < NUM_UARTS)) {
    pynq_error("invalid UART %d, must be 0..%d-1\n", uart, NUM_UARTS);
  }
  if (uart_ptrs[uart] == NULL) {
    pynq_error("UART%d has not been initialized.\n", uart);
  }
  return ((uart_ptrs[uart][UART_REG_STATUS] &
           UART_REG_STATUS_BIT_RX_FIFO_HAS_DATA) ==
          UART_REG_STATUS_BIT_RX_FIFO_HAS_DATA);
}

bool uart_has_space(const int uart) {
  if (!(uart >= UART0 && uart < NUM_UARTS)) {
    pynq_error("invalid UART %d, must be 0..%d-1\n", uart, NUM_UARTS);
  }
  if (uart_ptrs[uart] == NULL) {
    pynq_error("UART%d has not been initialized.\n", uart);
  }
  return ((uart_ptrs[uart][UART_REG_STATUS] &
           UART_REG_STATUS_BIT_TX_FIFO_FULL) == 0);
}

void uart_reset_fifos(const int uart) {
  if (!(uart >= UART0 && uart < NUM_UARTS)) {
    pynq_error("invalid UART %d, must be 0..%d-1\n", uart, NUM_UARTS);
  }
  if (uart_ptrs[uart] == NULL) {
    pynq_error("UART%d has not been initialized.\n", uart);
  }
  uart_ptrs[uart][UART_REG_CONTROL] = UART_REG_CONTROL_BIT_CLEAR_FIFOS;
}
