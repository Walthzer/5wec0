 /*
 *  TU/e 5WEC0::PYNQ RYB Module Testing. Motors Drivers module
 *
 *  Interface:
 *  Switch 1 -> Exits the main loop
 *  Switch 0: Changes our value selection
 *  Button 3: Resets the value
 *  Button 2: NC
 *  Button 1: Increases value
 *  Button 0: Decreases value
 * 
 *  OUTPUTS:
 *  IO_AR5 => Frequency PWM
 *  IO_AR6 => Amplitude PWM
 * 
 *  Written by: Walthzer
 * 
 */
#include <libpynq.h>
#include <stdlib.h>
#include <libui.h>
#include <libcom.h>

#define FREQ_PIN IO_AR5
#define AMPL_PIN IO_AR6
#define PWM_PERIOD 100000

typedef struct _data_ {
  int inputs[2];
  int freq_pwm, ampl_pwm;
} Data;

int clamp_1_5(int *val)
{
  *val = *val > 5 ? 5 : *val;
  *val = *val < 1 ? 1 : *val;

  return *val;
}

int dutycycle_percent(int index)
{
  static const int lookup[] = {5, 20, 40, 60, 80};

  //Clamp index between 1 and 5
  clamp_1_5(&index);

  return (lookup[index - 1]);
}
int dutycycle_period(int index)
{
  float decimal = 0.01 * dutycycle_percent(index);

  return (PWM_PERIOD - (PWM_PERIOD * decimal));
}


int main(void) {
  pynq_init();
  ui_t ui;
  ui_init(&ui);

  //Set header
  ui_rcenter(&ui, 0, true);
  ui_rprintf(&ui, 0, "%q%w%tRYB 24", RGB_WHITE, RGB_ORANGE, COMIC);
  ui_rcenter(&ui, 1, true);
  ui_rprintf(&ui, 1, "%q%w%tMotors Drivers", RGB_WHITE, RGB_ORANGE, COMIC);
  //interface layout
  //Inputs: Frequency and Amplitude index
  ui_rprintf(&ui, 2, "%q--Inputs--", RGB_CYAN);
  ui_rcenter(&ui, 2, true);
  //Outputs: Frequency PWM and Amplitude PWM
  ui_rprintf(&ui, 5, "%q--Outputs--", RGB_CYAN);
  ui_rcenter(&ui, 5, true);
  ui_draw(&ui);

  //Init inputs
  switches_init();
  buttons_init();

  //Init PWM
  switchbox_init();
  switchbox_set_pin(FREQ_PIN, SWB_PWM0);
  switchbox_set_pin(AMPL_PIN, SWB_PWM1);
  
  pwm_init(PWM0, PWM_PERIOD); //1Khz -> nanoseconds
  pwm_init(PWM1, PWM_PERIOD); //1Khz -> nanoseconds

  int state = 0;
  int button0 = get_button_state(0);
  int button1 = get_button_state(1);
  Data data = {{1, 1}, 0, 0};
  while (get_switch_state(1))
  {
    //Dynamic Values
    state = get_button_state(0);
    if(state != button0)
    {
      data.inputs[get_switch_state(0)]-= state;
      button0 = state;
    }
    state = get_button_state(1);
    if(state != button1)
    {
      data.inputs[get_switch_state(0)]+= state;
      button1 = state;
    }
    //Reset counter
    if(get_button_state(3))
      data.inputs[get_switch_state(0)] = 1;

    //inputs
    ui_rprintf(&ui, 3, "%qFrequency:[1-5]: %q%d", RGB_PURPLE, get_switch_state(0) ? RGB_WHITE : RGB_GREEN, clamp_1_5(&data.inputs[0]));
    ui_rprintf(&ui, 4, "%qAmplitude:[1-5]: %q%d", RGB_YELLOW, get_switch_state(0) ? RGB_GREEN : RGB_WHITE, clamp_1_5(&data.inputs[1]));


    //Adjust PWM outputs
    //Frequency
    if(data.freq_pwm != data.inputs[0])
    {
      data.freq_pwm = data.inputs[0];
      pwm_set_duty_cycle(PWM0, dutycycle_period(data.freq_pwm));
    }
    //Amplitude
    if(data.ampl_pwm != data.inputs[1])
    {
      data.ampl_pwm = data.inputs[1];
      pwm_set_duty_cycle(PWM1, dutycycle_period(data.ampl_pwm));
    }

    //outputs
    ui_rprintf(&ui, 6, "%qFreq_Duty[0-90]: %q%d", RGB_PURPLE, get_switch_state(0) ? RGB_WHITE : RGB_GREEN, dutycycle_percent(data.freq_pwm));
    ui_rprintf(&ui, 7, "%qAmpl_Duty[0-90]: %q%d", RGB_YELLOW, get_switch_state(0) ? RGB_GREEN : RGB_WHITE, dutycycle_percent(data.ampl_pwm));

    ui_draw(&ui);
  }

  //Cleanup PWM
  pwm_destroy(PWM0);
  pwm_destroy(PWM1);
  //Destroying switchbox is unnecessary
  buttons_destroy();
  switches_destroy();
  ui_destroy(&ui);
  pynq_destroy();

  return 0;
};