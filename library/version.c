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
#include <libpynq.h>

/**********************
 * WARNING
 * only change the numbers in these 4 #defs; do not change anything else
 * the libpynq version in doxygen ryb.doxy is updated automatically based
 * on the next 4 lines
 **********************/
#define LIBPYNQ_RELEASE "5EWC0-2023"
#define LIBPYNQ_VERSION_MAJOR 0
#define LIBPYNQ_VERSION_MINOR 2
#define LIBPYNQ_VERSION_PATCH 1
const version_t libpynq_version = {
    LIBPYNQ_RELEASE,
    LIBPYNQ_VERSION_MAJOR,
    LIBPYNQ_VERSION_MINOR,
    LIBPYNQ_VERSION_PATCH,
};

#undef LOG_DOMAIN
#define LOG_DOMAIN "version"

void print_version(void) {
  arm_shared t;
  version_t volatile *hardwareVersion =
      (version_t volatile *)arm_shared_init(&t, axi_version_0, 4096);
  printf("Bitstream version: %d.%d.%d\r\n", hardwareVersion->major,
         hardwareVersion->minor, hardwareVersion->patch);
  printf("Libpynq release %s version %d.%d.%d\r\n", libpynq_version.release,
         libpynq_version.major, libpynq_version.minor, libpynq_version.patch);
  if (libpynq_version.major != hardwareVersion->major) {
    pynq_error(
        "ERROR: the bitstream (hardware) and the libpynq library versions "
        "are incompatible. Please update your SD-card image and libpynq "
        "library.\n");
  } else if (libpynq_version.minor > hardwareVersion->minor) {
    printf("INFO: the libpynq library is newer than the bitstream (hardware). "
           "Please check if there is a newer version of the SD-card image.\n");
  } else if (libpynq_version.minor < hardwareVersion->minor) {
    printf(
        "INFO: the bitstream (hardware) is newer than the libpynq library. "
        "Please check if there is a newer version of the libpynq library.\n");
  }
  arm_shared_close(&t);
}

void check_version(void) {
  arm_shared t;
  version_t volatile *hardwareVersion =
      (version_t volatile *)arm_shared_init(&t, axi_version_0, 4096);
  if (libpynq_version.major != hardwareVersion->major) {
    print_version();
  }
  arm_shared_close(&t);
}
