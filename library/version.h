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
#ifndef VERSION_H
#define VERSION_H

/**
 * @defgroup VERSION Versioning library
 *
 * @brief Typedef and functions to check the version and compatibility of the
 * libpynq library and the FPGA bitstream.
 *
 * Semantic versioning (https://semver.org) is used.
 * Given a version number MAJOR.MINOR.PATCH, increment the:
 * - MAJOR version when you make incompatible API changes between libpynq and
 * FPGA bitstream (SD-card image)
 * - MINOR version when you add functionality in a backward compatible manner.
 * - PATCH version when you make backward compatible bug fixes.
 *
 * When the libpynq library version and the FPGA bitstream version are not the
 * same:
 * - libpynq.MAJOR < bitstream.MAJOR: you MUST update libpynq to the latest
 * version compatible with the bitstream version. The check_version function
 * will fail and exit your program.
 * - libpynq.MAJOR > bitstream.MAJOR: you MUST update the bitstream to the
 * latest version compatible with the libpynq version (or downgrade the libpynq
 * version to bitstream.MAJOR). The print/check_version function will fail
 * and exit your program.
 * - libpynq.MINOR > bitstream.MINOR: it is recommended to update the bitstream
 * to the latest version compatible with the libpynq version. The print_version
 * function will print an INFO message.
 * - libpynq.MINOR < bitstream.MINOR: it is recommended to update the libpynq to
 * the latest version compatible with the bitstream version. The print_version
 * function will print an INFO message.
 * - libpynq.PATCH != bitstream.PATCH: no action required
 *
 * @{
 */

#include <stdint.h>

/**
 * Typedef of version.
 */
typedef struct {
  uint8_t release[64];
  uint32_t major;
  uint32_t minor;
  uint32_t patch;
} version_t;

/**
 * Constant containing the version of this the libpynq library.
 */
extern const version_t libpynq_version;

/**
 * Print the version of the hardware (bitstream) and the libpynq library.
 *
 * Prints INFO message when minor/patch versions are different.
 */
extern void print_version(void);

/**
 * Check the version of the hardware (bitstream) and the libpynq library.
 * Called by e.g. the switchbox but can also be called in user code.
 *
 * @warning Fails with program exit when versions are incompatible.
 */
extern void check_version(void);

/**
 * @}
 */

#endif
