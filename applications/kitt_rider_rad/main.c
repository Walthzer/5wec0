#include <libpynq.h>

int main(void) {
  pynq_init();

  leds_init_onoff();
  printf("Wait for how many milliseconds? ");
  int wait;
  scanf("%d",&wait);
  int ledn = 0;
  int counter = 0;
  int n = 1;
  if (wait<0){
    green_led_onoff(0,LED_OFF);
    printf("The number of milliseconds should be at least 0.");
  }
  else{
    while(counter<20){
      if (ledn>3){
      ledn = n -(2*n+1);
      }
      green_led_onoff(ledn,LED_ON);
      sleep_msec(wait);
      green_led_onoff(ledn,LED_OFF);
      sleep_msec(wait);
      ledn++;
      counter++;
      n++;
      printf(ledn);
    }
  }
  
  leds_destroy();
  pynq_destroy();
  return EXIT_SUCCESS;
}

//I don't know how to use terminal :(