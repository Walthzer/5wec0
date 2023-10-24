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

#define COM_MMAPSIZE 8
#define COM_BASE_ADRESS 0x54
#define COM_IIC_INDEX 0
#define COM_MAXSLAVES 3

/**
 * @brief Module and buffer label enum
 * @warning Master is label 0
 */
enum com_labels
{
  DECISION,
  CRYING,
  MOTOR,
  HEARTBEAT
};

/**
 * @brief Internal type, do not use. Type of com that hold all
 * internal communication data
 */
typedef struct {
  uint8_t label;
  uint32_t mmap[COM_MMAPSIZE];
  uint32_t recv_buffer[COM_MMAPSIZE];
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
 * @brief Stages data for transmission
 * @param com Handle to the com.
 * @param label com_buffer_labels entry
 * @param data in uint32 format
 * @warning Does not transmit data on its own -> see com_run
 */
extern void com_put(com_t *com, int label, uint32_t data);

#endif
