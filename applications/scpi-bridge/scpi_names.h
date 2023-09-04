#pragma once

#include <pinmap.h>
// #include <scpi/scpi.h>
//  Switchbox defines
#include <switchbox.h>
// GPIO level
#include "driver_ads1115.h"
#include <gpio.h>

const scpi_choice_def_t ads1115_range_scpi[] = {
    {"V6P144", ADS1115_RANGE_6P144V},
    {"V4P096", ADS1115_RANGE_4P096V},
    {"V2P048", ADS1115_RANGE_2P048V},
    {"V1P024", ADS1115_RANGE_1P024V},
    {"V0P512", ADS1115_RANGE_0P512V},
    {"V0P256", ADS1115_RANGE_0P256V},
    SCPI_CHOICE_LIST_END /* termination of option list */
};
const scpi_choice_def_t ads1115_rate_scpi[] = {
    {"SPS8", ADS1115_RATE_8SPS},
    {"SPS16", ADS1115_RATE_16SPS},
    {"SPS32", ADS1115_RATE_32SPS},
    {"SPS64", ADS1115_RATE_64SPS},
    {"SPS128", ADS1115_RATE_128SPS},
    {"SPS250", ADS1115_RATE_250SPS},
    {"SPS475", ADS1115_RATE_475SPS},
    {"SPS860", ADS1115_RATE_860SPS},
    SCPI_CHOICE_LIST_END /* termination of option list */
};
const scpi_choice_def_t ads1115_channel_scpi[] = {
    {"A0TA1", ADS1115_CHANNEL_AIN0_AIN1},
    {"A0TA3", ADS1115_CHANNEL_AIN0_AIN3},
    {"A1TA3", ADS1115_CHANNEL_AIN1_AIN3},
    {"A2TA3", ADS1115_CHANNEL_AIN2_AIN3},
    {"A0TG", ADS1115_CHANNEL_AIN0_GND},
    {"A1TG", ADS1115_CHANNEL_AIN1_GND},
    {"A2TG", ADS1115_CHANNEL_AIN2_GND},
    {"A3TG", ADS1115_CHANNEL_AIN3_GND},
    SCPI_CHOICE_LIST_END /* termination of option list */
};

const scpi_choice_def_t pin_name_scpi[] = {
    /** 0 */
    {"AR0", SWB_AR0},
    /** 1 */
    {"AR1", SWB_AR1},
    /** 2 */
    {"AR2", SWB_AR2},
    /** 3 */
    {"AR3", SWB_AR3},
    /** 4 */
    {"AR4", SWB_AR4},
    /** 5 */
    {"AR5", SWB_AR5},
    /** 6 */
    {"AR6", SWB_AR6},
    /** 7 */
    {"AR7", SWB_AR7},
    /** 8 */
    {"AR8", SWB_AR8},
    /** 9 */
    {"AR9", SWB_AR9},
    /** 10 */
    {"AR10", SWB_AR10},
    /** 11 */
    {"AR11", SWB_AR11},
    /** 12 */
    {"AR12", SWB_AR12},
    /** 13 */
    {"AR13", SWB_AR13},
    /** 14 */
    {"A0", SWB_A0},
    /** 15 */
    {"A1", SWB_A1},
    /** 16 */
    {"A2", SWB_A2},
    /** 17 */
    {"A3", SWB_A3},
    /** 18 */
    {"A4", SWB_A4},
    /** 19 */
    {"A5", SWB_A5},

    /** 20 */
    {"SW0", SWB_SW0},
    /** 21 */
    {"SW1", SWB_SW1},
    /** 22 */
    {"BTN0", SWB_BTN0},
    /** 23 */
    {"BTN1", SWB_BTN1},
    /** 24 */
    {"BTN2", SWB_BTN2},
    /** 25 */
    {"BTN3", SWB_BTN3},
    /** 26 */
    {"LD0", SWB_LD0},
    /** 27 */
    {"LD1", SWB_LD1},
    /** 28 */
    {"LD2", SWB_LD2},
    /** 29 */
    {"LD3", SWB_LD3},
    /** 30 */
    {"AR_SDA", SWB_AR_SDA},
    /** 31 */
    {"AR_SCL", SWB_AR_SCL},
    /** 32 */
    {"LD4B", SWB_LD4B},
    /** 33 */
    {"LD4G", SWB_LD4G},
    /** 34 */
    {"LD4R", SWB_LD4R},
    /** 35 */
    {"LD5B", SWB_LD5B},
    /** 36 */
    {"LD5G", SWB_LD5G},
    /** 37 */
    {"LD5R", SWB_LD5R},

    /** 38 */
    {"RBPI40", SWB_RBPI40},
    /** 39 */
    {"RBPI37", SWB_RBPI37},
    /** 40 */
    {"RBPI38", SWB_RBPI38},
    /** 41 */
    {"RBPI35", SWB_RBPI35},
    /** 42 */
    {"RBPI36", SWB_RBPI36},
    /** 43 */
    {"RBPI33", SWB_RBPI33},
    /** 44 */
    {"RBPI18", SWB_RBPI18},
    /** 45 */
    {"RBPI32", SWB_RBPI32},
    /** 46 */
    {"RBPI10", SWB_RBPI10},
    /** 47 */
    {"RBPI27", SWB_RBPI27},
    /** 48 */
    {"RBPI28", SWB_RBPI28},
    /** 49 */
    {"RBPI22", SWB_RBPI22},
    /** 50 */
    {"RBPI23", SWB_RBPI23},
    /** 51 */
    {"RBPI24", SWB_RBPI24},
    /** 52 */
    {"RBPI21", SWB_RBPI21},
    /** 53 */
    {"RBPI26", SWB_RBPI26},
    /** 54 */
    {"RBPI19", SWB_RBPI19},
    /** 55 */
    {"RBPI31", SWB_RBPI31},
    /** 56 */
    {"RBPI15", SWB_RBPI15},
    /** 57 */
    {"RBPI16", SWB_RBPI16},
    /** 58 */
    {"RBPI13", SWB_RBPI13},
    /** 59 */
    {"RBPI12", SWB_RBPI12},
    /** 60 */
    {"RBPI29", SWB_RBPI29},
    /** 61 */
    {"RBPI08", SWB_RBPI08},
    /** 62 */
    {"RBPI07", SWB_RBPI07},
    /** 63 */
    {"RBPI05", SWB_RBPI05},
    SCPI_CHOICE_LIST_END /* termination of option list */
};

const scpi_choice_def_t gpio_level_name_scpi[] = {
    {"Low", GPIO_LEVEL_LOW},
    {"High", GPIO_LEVEL_HIGH},
    /** 0 */
    SCPI_CHOICE_LIST_END /* termination of option list */
};

const scpi_choice_def_t gpio_dir_name_scpi[] = {
    {"INput", GPIO_DIR_INPUT},
    {"OUTput", GPIO_DIR_OUTPUT},
    /** 0 */
    SCPI_CHOICE_LIST_END /* termination of option list */
};

typedef enum InterruptEdge {
  RISING_EDGE = 1,
  FALLING_EDGE = 2,
  BOTH_EDGE = 3,
} InterruptEdge;

const scpi_choice_def_t counter_edge_scpi[] = {
    {"Rising", RISING_EDGE}, {"Falling", FALLING_EDGE}, {"Both", BOTH_EDGE}};

const scpi_choice_def_t adc_channel_name_scpi[] = {
    {"A0", ADC0}, {"A1", ADC1}, {"A2", ADC2},        {"A3", ADC3},
    {"A4", ADC4}, {"A5", ADC5}, SCPI_CHOICE_LIST_END /* termination of option
                                                        list */
};

const scpi_choice_def_t switchbox_name_scpi[] = {
    /** Map pin to GPIO */
    {"GPIO", SWB_GPIO},
    /** Map pin to TX channel of UART 0 */
    {"UART0_TX", SWB_UART0_TX},
    /** Map pin to RX channel of UART 0 */
    {"UART0_RX", SWB_UART0_RX},
    /** Map pin to clock channel of SPI 0 */
    {"SPI0_CLK", SWB_SPI0_CLK},
    /** Map pin to miso channel of SPI 0 */
    {"SPI0_MISO", SWB_SPI0_MISO},
    /** Map pin to mosi channel of SPI 0 */
    {"SPI0_MOSI", SWB_SPI0_MOSI},
    /** Map pin to ss channel of SPI 0 */
    {"SPI0_SS", SWB_SPI0_SS},
    /** Map pin to clock channel of SPI 1 */
    {"SPI1_CLK", SWB_SPI1_CLK},
    /** Map pin to miso channel of SPI 1 */
    {"SPI1_MISO", SWB_SPI1_MISO},
    /** Map pin to mosi channel of SPI 1 */
    {"SPI1_MOSI", SWB_SPI1_MOSI},
    /** Map pin to ss channel of SPI 1 */
    {"SPI1_SS", SWB_SPI1_SS},
    /** Map pin to sda channel of IIC 0 */
    {"IIC0_SDA", SWB_IIC0_SDA},
    /** Map pin to scl channel of IIC 0 */
    {"IIC0_SCL", SWB_IIC0_SCL},
    /** Map pin to sda channel of IIC 1 */
    {"IIC1_SDA", SWB_IIC1_SDA},
    /** Map pin to scl channel of IIC 1 */
    {"IIC1_SCL", SWB_IIC1_SCL},
    /** Map pin to output channel of PWM 0 */
    {"PWM0", SWB_PWM0},
    /** Map pin to output channel of PWM 1 */
    {"PWM1", SWB_PWM1},
    /** not connected */
    {"PWM2", SWB_PWM2},
    /** not connected */
    {"PWM3", SWB_PWM3},
    /** not connected */
    {"PWM4", SWB_PWM4},
    /** not connected */
    {"PWM5", SWB_PWM5},
    {"TIMER_G0", SWB_TIMER_G0},
    {"TIMER_G1", SWB_TIMER_G1},
    /** not connected */
    {"TIMER_G2", SWB_TIMER_G2},
    /** not connected */
    {"TIMER_G3", SWB_TIMER_G3},
    /** not connected */
    {"TIMER_G4", SWB_TIMER_G4},
    /** not connected */
    {"TIMER_G5", SWB_TIMER_G5},
    /** not connected */
    {"TIMER_G6", SWB_TIMER_G6},
    /** not connected */
    {"TIMER_G7", SWB_TIMER_G7},
    {"UART1_TX", SWB_UART1_TX},
    {"UART1_RX", SWB_UART1_RX},
    {"TIMER_IC0", SWB_TIMER_IC0},
    {"TIMER_IC1", SWB_TIMER_IC1},
    {"TIMER_IC2", SWB_TIMER_IC2},
    {"TIMER_IC3", SWB_TIMER_IC3},
    {"TIMER_IC4", SWB_TIMER_IC4},
    {"TIMER_IC5", SWB_TIMER_IC5},
    {"TIMER_IC6", SWB_TIMER_IC6},
    {"TIMER_IC7", SWB_TIMER_IC7},
    SCPI_CHOICE_LIST_END /* termination of option list */
};
