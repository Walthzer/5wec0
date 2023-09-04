#include <uart.h>
void uart_init(const int uart){};
void uart_destroy(const int uart){};
void uart_send(const int uart, const uint8_t data){};
uint8_t uart_recv(const int uart){};
bool uart_has_data(const int uart){};
bool uart_has_space(const int uart){};
void uart_reset_fifos(const int uart){};
