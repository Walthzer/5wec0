#include <adc.h>
#include <arm_shared_memory_system.h>
bool initialized_adc(void){};
void adc_init(void){};
void adc_destroy(void){};
double adc_read_channel(adc_channel_t channel){};
uint32_t adc_read_channel_raw(adc_channel_t channel){};
