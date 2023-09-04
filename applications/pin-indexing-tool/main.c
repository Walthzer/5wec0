#include <stdio.h>

#include <libpynq.h>

void printInfo(void) {
  printf("?: this help\n");
  printf("p: prints map pin versus channel name\n");
  printf("m: map the entire switchbox using the switchbox_debug function\n");
  printf("s: set a pin to a GPIO output pin and set it either high or low\n");
  printf("r: resets the entire switchbox\n");
  printf("l: link a pin to a channel\n");
  printf("d: sub-menu for all kinds of PWM settings\n");
  printf("    d: set duty cycle for a certain channel\n");
  printf("    f: initialize channel\n");
  printf("    s: print state of all channels\n");
  printf("    r: remove all parameters from channel\n");
  printf("    q: return to main menu\n");
  printf("q : quit the application\n");
}

void printInfoPWM(void) {
  printf("    d: set duty cycle for a certain channel\n");
  printf("    f: initialize channel\n");
  printf("    s: get state of all channels\n");
  printf("    r: remove all parameters from channel\n");
  printf("    q: return to main menu\n");
}

void printMapIndex(void) {
  printf("Pin #\tSwitchbox name\n");
  for (int i = 0; i < NUM_SWITCHBOX_NAMES; i++) {
    printf("%i\t%s\n", i, switchbox_names[i]);
  }
}

int main(int argc __attribute__((unused)),
         char **argv __attribute__((unused))) {
  pynq_init();

  uint8_t channel, pin_number, pwm;
  char state, cmd;

  printInfo();
  while (1) {
    printf("Command? ");
    int r = scanf(" %c", &cmd);
    if ( r == EOF ) {
	    break;
    }
    switch (cmd) {
    case '?':
      printInfo();
      break;
    case 'l':
      printf("Pin channel?\n");
      scanf(" %hhi %hhi", &pin_number, &channel);
      switchbox_set_pin(pin_number, channel);
      break;
    case 'p':
      printMapIndex();
      break;
    case 'm':
      mapping_info();
      break;
    case 'r':
      printf("Resetting switchbox...   ");
      gpio_init();
      gpio_reset();
      switchbox_init();
      switchbox_reset();
      printf("Done\n");
      break;
    case 's':
      printf("Channel state?\n");
      scanf(" %hhi %c", &channel, &state);
      if (state == 'h') {
        gpio_set_direction(channel, GPIO_DIR_OUTPUT);
        gpio_set_level(channel, GPIO_LEVEL_HIGH);
      } else if (state == 'l') {
        gpio_set_direction(channel, GPIO_DIR_OUTPUT);
        gpio_set_level(channel, GPIO_LEVEL_LOW);
      } else {
        printf("Invalid state '%c' for channel %i, state can only be h (HIGH) "
               "or l (LOW)\n",
               state, channel);
      }
      break;
    case 'd':
      printInfoPWM();
      while (cmd != 'q') {
        printf("PWM command? ");
        r = scanf(" %c", &cmd);
	if ( r == EOF ) {
		cmd = 'q';
	}
        switch (cmd) {
        case '?':
          printInfoPWM();
          break;
        case 'd':
          printf("Channel (0-5)  Duty-Cycle (0-255)\n");
          scanf(" %hhi %hhi", &channel, &pwm);
          if (pwm_initialized(channel)) {
            pwm_set_duty_cycle(channel, pwm);
          } else {
            printf("Channel %i has not been initialized\n", channel);
          }
          break;
        case 'f':
          printf("Initialize which channel (0-5)? ");
          scanf(" %hhi", &channel);
          pwm_init(channel, 256);
          break;
        case 's':
          for (int i = 0; i < NUM_PWMS; i++) {
            if (pwm_initialized(i) == true) {
              printf("Channel    %i\n", i);
              printf("Period     %hhi\n", pwm_get_period(i));
              printf("Duty       %hhi\n", pwm_get_duty_cycle(i));
              printf("Steps      %hhi\n", pwm_get_steps(i));
            } else {
              printf("Channel %i has not been initialized\n", i);
            }
          }
          break;
        case 'r':
          printf("Destroy which channel (0-5)?\n");
          scanf(" %hhi", &channel);
          pwm_destroy(channel);
          break;
        case 'q':
          printf("Returning to main menu\n");
          break;
        default:
          printf("Invalid command '%c'\n", cmd);
          break;
        }
      }
      break;
    case 'q':
      printf("Goodbye\n");
      gpio_destroy();
      switchbox_destroy();
      return EXIT_SUCCESS;
      break;
    default:
      printf("Invalid command '%c'\n", cmd);
      break;
    }
  }

  pynq_destroy();
  return EXIT_SUCCESS;
}
