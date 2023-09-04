#include <libpynq.h>

#if 0
#define debug(x, ...) printf(x, ##__VA_ARGS__)
#else
#define debug(x, ...)
#endif

uint8_t *getMessage(const uart_index_t uart) {
  int index = 0;
  uint8_t *msg = malloc(sizeof(uint8_t) * 40), c;
  for (int i = 0; i < 40; i++) {
    msg[i] = '\0';
  }
  do {
    c = uart_recv(uart);
    msg[index] = c;
    index++;
  } while (c != '\n');
  debug("Exiting loop, returning msg pointer\n");
  return msg;
}

void uart_send_string(const uart_index_t uart, const char *msg) {
  debug("SENT: ");
  for (uint32_t i = 0; i < strlen(msg); i++) {
    uint8_t nextAscii = msg[i];

    bool keepTrying = true;
    while (keepTrying) {
      if (uart_has_space(uart)) {
        uart_send(uart, nextAscii);
        keepTrying = false;
        debug("%c", nextAscii);
      }
    }
  }
}

void receiver(const uart_index_t uart) {
  // RECEIVING AND HAVING SCREEN AS MASTER
  display_t dev;
  display_init(&dev);
  displayFillScreen(&dev, RGB_BLACK);
  uint8_t x = 5, y = 5, newX = 5, newY = 5;
  static uint8_t width = 20, height = 20;
  displayFillScreen(&dev, RGB_BLACK);
  displayDrawFillRect(&dev, x * width, y * height, x * width + width,
                      y * height + height, RGB_WHITE);

  while (1) {
    if (uart_has_data(uart)) {
      uint8_t *msg = getMessage(uart);
      debug("Received: %s", msg);

      // Updating coords
      if (x < 11 && msg[0] == '1')
        newX += 1;
      if (x > 0 && msg[1] == '1')
        newX -= 1;
      if (y < 11 && msg[2] == '1')
        newY += 1;
      if (y > 0 && msg[3] == '1')
        newY -= 1;

      // update screen
      if (newX != x || newY != y) {
        x = newX;
        y = newY;
        displayFillScreen(&dev, RGB_BLACK);
        displayDrawFillRect(&dev, x * width, y * height, x * width + width,
                            y * height + height, RGB_WHITE);
        debug("X: %i\tY:%i\n", x, y);
      }
      free(msg);
    }
  }
}

void sender() {
  // RECEIVING MESSAGE AS SLAVE
  debug("SENDING: ");
  while (1) {
    int btnLvsl[NUM_BUTTONS];
    sleep_msec_buttons_pushed(btnLvsl, 100);
    char btnSt[5];
    btnSt[NUM_BUTTONS] = '\n';
    for (int i = 0; i < NUM_BUTTONS; i++)
      btnSt[i] = (btnLvsl[i] == BUTTON_NOT_PUSHED ? '0' : '1');
    if (uart_has_space(UART0)) {
      uart_send_string(UART0, btnSt);
    } else {
      debug("UART has no space to send any data...\n");
    }
    sleep_msec(10);
  }
}

int main(void) {
  pynq_init();

  // Setting direciton of switches
  gpio_set_direction(SWB_SW0, GPIO_DIR_INPUT);
  gpio_set_direction(SWB_BTN0, GPIO_DIR_INPUT);

  printf("Please set state to MASTER or SLAVE using SW0 and press BTN0...\n");
  printf("Set current state to ");
  bool mode = gpio_get_level(SWB_SW0);
  if (mode) {
    printf("MASTER\n"); }
  else {
    printf("SLAVE\n");
  }
  do {
    if (mode != gpio_get_level(SWB_SW0)) {
      printf("Set current state to ");
      mode = gpio_get_level(SWB_SW0);
      if (mode) {
        printf("MASTER\n"); }
      else {
        printf("SLAVE\n");
      }
    }
    sleep_msec(10);
  } while (gpio_get_level(SWB_BTN0) != GPIO_LEVEL_HIGH);

  printf("Starting UART transmission as ");
  if (mode) {
    printf("MASTER\n"); }
  else {
    printf("SLAVE\n");
  }

  // Initializing routing in the switchbox and routing it to the UART channel
  switchbox_set_pin(SWB_AR0, SWB_UART0_RX);
  switchbox_set_pin(SWB_AR1, SWB_UART0_TX);

  uart_init(UART0);

  uart_reset_fifos(UART0);

  if (mode) {
    receiver(UART0);
  } else {
    sender();
  }

  pynq_destroy();
  return EXIT_SUCCESS;
}
