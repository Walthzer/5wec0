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
#ifndef UART_H
#define UART_H
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup UART UART library
 *
 * @brief Functions to use the Universal Asynchronous Receiver-Transmitter
 * (UART).
 *
 * Two UART channels can be instantiated, UART0 and UART1.
 * Before sending and receiving bytes the UART must be connect to some I/O pins
 * through the switchbox, e.g.
 * @code
 * switchbox_set_pin(IO_AR0, SWB_UART0_RX);
 * switchbox_set_pin(IO_AR1, SWB_UART0_TX);
 * @endcode
 *
 * After that, an example of how to use this library for the MASTER.
 * @code
 * #include <libpynq.h>
 * int main (void)
 * {
 *  // initialise all I/O
 *  pynq_init();
 *
 *  // initialize UART 0
 *  uart_init(UART0);
 *  // flush FIFOs of UART 0
 *  uart_reset_fifos(UART0);
 *
 *  uint8_t byte[] = "Hello\n";
 *  int i = 0;
 *  while (byte[i] != '\0') {
 *   uart_send (UART0, byte[i]);
 *   printf("sent byte %d\n", byte[i]);
 *   i++;
 *  }
 *
 *   // clean up after use
 *   pynq_destroy();
 *   return EXIT_SUCCESS;
 * }
 * @endcode
 *
 * An example of how to use this library for the SLAVE.
 * @code
 * #include <libpynq.h>
 * int main (void)
 * {
 *  // initialise all I/O
 *  pynq_init();
 *
 *  // initialize UART channel 0
 *  uart_init(UART0);
 *  // flush FIFOs of UART 0
 *  uart_reset_fifos (UART0);
 *
 *  printf("listening\n");
 *  do {
 *    // get a byte from UART 0
 *    uint8_t msg = uart_recv(UART0);
 *    printf("received byte %d\n", msg);
 *  } while (1);
 *
 *   // clean up after use
 *   pynq_destroy();
 *   return EXIT_SUCCESS;
 * }
 * @endcode
 *
 * UARTs can be routed through the switch box (see switchbox.h). Note that
 * switchbox numbering (SWB_UART0..SWB_UART1) is then used instead of
 * 0..NUM_UARTS-1 (UART0..UART1).
 *
 * @{
 */

/**
 * @brief Enum of UARTs.
 * Functions use a switch numbered from 0..NUM_UARTS-1.
 * Alternatively, you can use UARTi instead of just i if you find that
 * clearer.
 */
typedef enum { UART0 = 0, UART1 = 1, NUM_UARTS } uart_index_t;

/**
 * @param uart The UART index to initialize.
 * @brief Initialize the UART specified by the index with a shared memory handle
 * and a buffer size of 4096 bytes.
 * @warning Fails with program exit if the UART channel is outside valid range
 * or when the shared memory system has not been instantiated.
 */
extern void uart_init(const int uart);

/**
 * @brief Close the shared memory handle for the specified UART index.
 * @param uart The UART index to remove from the shared memory space.
 * @warning Fails with program exit if the UART channel is outside valid range.
 */
extern void uart_destroy(const int uart);

/**
 * @brief Send a byte of data on the specified UART index by waiting for the
 * transmit FIFO to have space and then writing the data to the transmit buffer.
 * @param uart The UART index to send data to.
 * @param data The data to send to the UART index.
 * @warning Fails with program exit if the UART channel is outside valid range.
 */
extern void uart_send(const int uart, const uint8_t data);

/**
 * @brief Receive a byte of data from the specified UART index by waiting for
 * the receive FIFO to have data and then reading the data from the receive
 * buffer.
 * @param uart The UART index to receive data from.
 * @return The received data byte.
 * @warning Fails with program exit if the UART channel is outside valid range.
 */
extern uint8_t uart_recv(const int uart);

/**
 * @brief Check if the receive FIFO for the specified UART index has data
 * available.
 * @param uart The UART index used to check for data.
 * @return True if the receive FIFO has data, false otherwise.
 * @warning Fails with program exit if the UART channel is outside valid range.
 */
extern bool uart_has_data(const int uart);

/**
 * @brief Check if the transmit FIFO for the specified UART index has space
 * available.
 * @param uart The UART index to check for space.
 * @return True if the FIFO has space, false otherwise.
 * @warning Fails with program exit if the UART channel is outside valid range.
 */
extern bool uart_has_space(const int uart);

/**
 * @brief This function resets both the transmit and receive FIFOs of the UART
 * specified by the `uart` parameter. This can be useful when there is data
 * stuck in the FIFOs or when the FIFOs are not behaving as expected.
 * @param uart The UART index of the UART whose FIFOs should be reset.
 * @warning This function is specific to UARTs that have FIFOs, and will have no
 * effect on UARTs that do not have FIFOs.
 * @warning Resetting the FIFOs will result in the loss of any data that is
 * currently in the FIFOs. Therefore, this function should be used with caution,
 * and only when it is absolutely necessary to do so.
 * @warning Fails with program exit if the UART channel is outside valid range.
 */
extern void uart_reset_fifos(const int uart);

/**
 * @}
 */

#endif // UART_H
