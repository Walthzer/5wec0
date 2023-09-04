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
#ifndef __ARM_SHARED_MEMORY_SYSTEMH_
#define __ARM_SHARED_MEMORY_SYSTEMH_

/**
 * @defgroup ARMSHARED ARM MMIO library
 *
 * @brief Do not use. Low-level functions for MMIO access to the FPGA fabric.
 *
 * This library gives low-level memory-mapped access to the hardware units in
 * the FPGA.
 *
 * This is an internal library and should not be directly used.
 *
 * @{
 */
#include <stdint.h>

struct arm_shared_t {
  int file_descriptor;
  uint32_t address;
  uint32_t length;
  void *mmaped_region;
};
/**
 * Object handle.
 */
typedef struct arm_shared_t arm_shared;

/**
 * @param handle a handle to store it internal state.
 * @param address address to access (should be in the shared memory range).
 * @param length the length of the section to access.
 *
 * Open a shared memory for reading and writing.
 *
 * @returns a pointer to the shared memory region.
 */

extern void *arm_shared_init(arm_shared *handle, const uint32_t address,
                             const uint32_t length);

/**
 * @param handle a handle to its internal state.
 *
 * closes the shared memory region, invalidating the previously accessed
 * pointer.
 */
extern void arm_shared_close(arm_shared *handle);

/**
 * @}
 */
#endif // ARM_READ_SHARED_H
