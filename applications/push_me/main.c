#include <libpynq.h>

int do_prompt(void) {
    printf("enter a number? ");
    int resultBuffer;
    scanf("%d", &resultBuffer);

    if (resultBuffer < 0)
    {
        printf("the number must be positive \n");
        return do_prompt();
    }

    return resultBuffer;
}

void stopPynq(void)
{
    // stop using the LEDs and PYNQ board
    leds_destroy();
    pynq_destroy();
}

int clamp_value(int x)
{   
    //DO NOT USE -> DOING BURNOUTS ON THE CPU WITH THIS => USE IF's
    /**
     * Formula:
     * 
     * Clamp a value x between 0-3.
     * Mirror into 3-0 if x < 0;
     * 
     * Case A: x >= 0
     *      f(0) = 0
     *      f(1) = 1
     *      f(2) = 2
     *      f(3) = 3
     *      f(4) = 0
     * 
     * Case B: x < 0
     *      f(0) = 0
     *      f(-1) = 1
     *      f(-2) = 2
     *      f(-3) = 3
     *      f(-4) = 0

    **/
    return ((x % 4 ) * (x >= 0)) + (3 * (abs(x) % 4) % 4) * (x < 0);
}

int main (void) 
{
    // we must first initialise the PYNQ board and the LEDs
    pynq_init();
    leds_init_onoff();
    buttons_init();

    //Init variables
    int counter = -1;

    int result = do_prompt();
    if (result < 0)
    {
        stopPynq();
        return EXIT_FAILURE;
    }
    counter = clamp_value(result);
    green_led_onoff(counter, 1);

    // now your program can use the LEDs and Buttons
    while (true) {
        //Halt until button press:
        int button = wait_until_any_button_pushed();
        wait_until_button_released(button);

        if (button > 1)
        {
            continue;
        }

        //Increment upon button_1. Decrement upon button_0;
        int modifier = - 1 + (2 * button);
        //Save memory by outputting twice.
        printf("%d", counter);
        green_led_onoff(counter, 0);
        counter = clamp_value(counter + modifier);
        printf("%c1 = %d\n", '-' * (modifier == -1) + '+' * (modifier == 1), counter);
        green_led_onoff(counter, 1);
    }
    

    stopPynq();
    return EXIT_SUCCESS;
}

