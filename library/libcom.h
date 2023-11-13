 /*
 *  TU/e 5WEC0::PYNQ Display UI system
 *
 *  Written by: Walthzer
 * 
 */
#include <fontx.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//PYNQ libs
#include <display.h>

#ifndef COM_H
#define COM_H

/**
 * @defgroup COM communication library
 *
 * @brief Communication interface for the PYNQ using IIC
 *
 * Define a com_t com (called the com "handle"), initialise it,
 * and pass this as the first parameter to all functions.
 *
 */

#define COM_BUFFERSIZE 8
#define COM_BASE_ADRESS 0x54
#define COM_IIC_INDEX 0
#define COM_MAXSLAVES 3

//Bounderies for signals
#define COM_HEARTBEAT_LOW_B 60
#define COM_HEARTBEAT_HIGH_B 240

#define COM_CRYING_LOW_B 0
#define COM_CRYING_HIGH_B 100

#define COM_MOTOR_LOW_B 1
#define COM_MOTOR_HIGH_B 5


/**
 * @brief Module and buffer label enum
 * @warning Master is label 0
 */
enum com_labels
{
  DECISION,
  CRYING,
  HEARTBEAT,
  MOTOR
};

/**
 * @brief Internal type, do not use. Type of com that hold all
 * internal communication data
 * 
 * @note Slaves need to have a uint32 buffer
 *  unlike the master who uses a uint8 buffer
 *  recv_buffer is used to check if data is new
 */
typedef struct {
  uint8_t label;
  uint8_t mmap[COM_BUFFERSIZE*4];
  uint32_t slave_buffer[COM_BUFFERSIZE];
  uint32_t recv_buffer[COM_BUFFERSIZE];
} com_t;

/**
 * @brief Initialize the communication system.
 * @param com Handle to the com.
 * @param label our ide
 */
extern void com_init(com_t* com_p, int label);

/**
 * @brief Clean up the communication system.
 * @param com Handle to the com.
 */
extern void com_destroy(com_t* com);

/**
 * @brief Perform send and receive operations
 * @param com Handle to the com.
 */
extern void com_run(com_t* com);

/**
 * @brief Get data from the buffer
 * @param com Handle to the com.
 * @param label com_buffer_labels entry
 * @param data_p pointer to update with data
 * @returns if the data was new or not
 * @warning buffer can be outdated -> see com_run 
 */
extern bool com_get(com_t *com, int label, uint32_t *data_p);

/**
 * @brief Extended com_get for the frequency and amplitude signals
 * @param com Handle to the com.
 * @param label com_buffer_labels entry
 * @param freq pointer to update with frequency unint16_t format
 * @param ampl pointer to update with amplitude unint16_t format
 * @returns if the data was new or not
 * @warning buffer can be outdated -> see com_run 
 */
extern bool com_getm(com_t *com, int label, uint16_t *freq_p, uint16_t *ampl_p);

/**
 * @brief Stages data for transmission
 * @param com Handle to the com.
 * @param label com_buffer_labels entry
 * @param data in uint32 format
 * @returns true if the data was within bounds
 * and put succesfully else false
 * @warning Does not transmit data on its own -> see com_run
 */
extern bool com_put(com_t *com, int label, uint32_t data);

/**
 * @brief Extended com_put for the frequency and amplitude signals
 * @param com Handle to the com.
 * @param label com_buffer_labels entry
 * @param freq frequency unint16_t format
 * @param ampl amplitude unint16_t format
 * @returns true if the freq and ampl were within bounds
 * and put succesfully else false
 * @warning Does not transmit data on its own -> see com_run
 */
extern bool com_putm(com_t *com, int label, uint16_t freq, uint16_t ampl);

#endif
