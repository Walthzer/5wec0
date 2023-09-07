#include <libpynq.h>

#define CHASER_REPETITION 20

int do_milisecond_prompt(void) {
    printf("Wait for how many milliseconds? ");
    int resultBuffer;
    scanf("%d", &resultBuffer);

    if (resultBuffer < 0)
    {
        printf("The number of milliseconds should be at least 0.");
        return -1;
    }

    return resultBuffer;
}

void stopPynq(void)
{
    // stop using the LEDs and PYNQ board
    leds_destroy();
    pynq_destroy();
}

void kittChaser(int delay, int end)
{
    //Send out the chaser and pull it back in when we reach the last LED.
    //Last iteration needs to light up LED_0 
    const int blinks = (NUM_GREEN_LEDS * 2) - 2 + end;
    int i = 0;
    while (i < blinks)
    {
        int modu = (i / NUM_GREEN_LEDS);
        int active_led = (abs(6-i) * modu) + (i - i * modu);
        green_led_onoff(active_led,LED_ON);
        sleep_msec(delay);
        green_led_onoff(active_led,LED_OFF);
        i++;
    }
}

int main (void) 
{
    // we must first initialise the PYNQ board and the LEDs
    pynq_init();
    leds_init_onoff();

    //Init variables
    int delayMili = 1000;

    int result = do_milisecond_prompt();
    if (result < 0)
    {
        stopPynq();
        return EXIT_SUCCESS;
    }
    delayMili = result;


    // now your program can use the LEDs
    int i = 0;
    while (i < CHASER_REPETITION)
    {
        kittChaser(delayMili, 1 - (i < CHASER_REPETITION -1));
        i++;
    }
    

    stopPynq();
    return EXIT_SUCCESS;
}

