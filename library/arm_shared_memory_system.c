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
#include <arm_shared_memory_system.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>

void *arm_shared_init(arm_shared *handle, const uint32_t address,
                      const uint32_t length) {
  if (handle == NULL) {
    fprintf(stderr, "You need to pass a valid handle to %s\n", __FUNCTION__);
    exit(EXIT_FAILURE);
  }

  handle->address = address;
  handle->length = length;
  handle->file_descriptor = open("/dev/mem", O_RDWR | O_SYNC);
  if (handle->file_descriptor < 0) {
    fprintf(stderr,
            "FAILED open memory: %s, please run with sufficient permissions "
            "(sudo).\n",
            strerror(errno));
    exit(EXIT_FAILURE);
  }

  long page_size = sysconf(_SC_PAGE_SIZE);

  uint32_t start_address = handle->address;
  uint32_t page_offset = start_address % page_size;
  start_address -= page_offset;
  handle->length += page_offset;

  handle->mmaped_region =
      mmap(NULL, handle->length, PROT_READ | PROT_WRITE, MAP_SHARED,
           handle->file_descriptor, start_address);

  if (handle->mmaped_region == MAP_FAILED) {
    fprintf(stderr, "FAILED to memory map requested region: %s\n",
            strerror(errno));
    close(handle->file_descriptor);
    exit(EXIT_FAILURE);
  }
  return (void *)(((uint32_t)(handle->mmaped_region)) + page_offset);
}

void arm_shared_close(arm_shared *handle) {
  if (handle == NULL) {
    fprintf(stderr, "You need to pass a valid handle to %s\n", __FUNCTION__);
    exit(EXIT_FAILURE);
  }
  if (handle->mmaped_region != MAP_FAILED) {
    munmap(handle->mmaped_region, handle->length);
  }
  if (handle->file_descriptor >= 0) {
    close(handle->file_descriptor);
  }
}
