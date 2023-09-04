/*
Copyright (c) 2023 Eindhoven University of Technology

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef IIC_H
#define IIC_H
#include <stdbool.h>
#include <stdint.h>

/**
 * @defgroup IIC IIC library
 *
 * @brief Functions to use the Inter-Integrated Circuit (IIC).
 *
 * High-level functions to read/write to clients connected to the two
 * integrated IIC modules.
 *
 * @{
 **/

/**
 * @brief Enum of IICs.
 * Functions use a switch numbered from 0..NUM_IICS-1.
 */
typedef enum { IIC0 = 0, IIC1 = 1, NUM_IICS = 2 } iic_index_t;

/**
 * @param uart The IIC index to initialize.
 * @brief Initialize the IIC specified by the index with a shared memory handle
 * and a buffer size of 4096 bytes.
 * @warning Fails with program exit if the IIC channel is outside valid range
 * or when the shared memory system has not been instantiated.
 */
extern void iic_init(const iic_index_t iic);

/**
 * @brief Close the shared memory handle for the specified IIC index.
 * @param uart The IIC index to remove from the shared memory space.
 * @warning Fails with program exit if the IIC channel is outside valid range.
 */
extern void iic_destroy(const iic_index_t iic);

/**
 * @param iic The IIC index to initialize.
 * @param addr The IIC address of the client to access.
 * @param reg The clients register address.
 * @param data Buffer where the register content is stored. [out]
 * @param length The amount of data to read.
 *
 * Reads the content of the register into data.
 *
 * @return 0 if successful, 1 on error
 */
extern bool iic_read_register(const iic_index_t iic, const uint8_t addr,
                              const uint8_t reg, uint8_t *data,
                              uint16_t length);

/**
 * @param iic The IIC index to initialize.
 * @param addr The IIC address of the client to access.
 * @param reg The clients register address.
 * @param data Buffer where new the register content is stored.
 * @param length The amount of data to write.
 *
 * Writes data to register.
 *
 * @return 0 if successful, 1 on error
 */
extern bool iic_write_register(const iic_index_t iic, const uint8_t addr,
                               const uint8_t reg, uint8_t *data,
                               uint16_t length);

/**
 * @}
 */
#endif
