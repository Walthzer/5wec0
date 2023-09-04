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
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

/** Logging domain for this file. */
#define DOMAIN "LOGGER"

/** Color codes for each log level */
static const char color_escape_calls[NUM_LOG_LEVELS][8] = {
    /** blue */
    "\033[1;32m",
    /** Yellow */
    "\033[1;33m",
    /** Red */
    "\033[1;31m"};
/** Name/prefix for each level */
static const char log_level_name[NUM_LOG_LEVELS][10] = {
    "INFO:    ", "WARNING: ", "ERROR:   "};
/** Reset coloring. */
static const char color_escape_blue[] = "\033[1;34m";
static const char color_escape_reset[] = "\033[0m";

static bool pynq_log_init = false;
static LogLevel critical_level = LOG_LEVEL_ERROR;
static LogLevel min_log_level = LOG_LEVEL_WARNING;

void pynq_log(const LogLevel level, char const *domain, char const *location,
              unsigned int lineno, char const *fmt, ...) {
  va_list arg_list;

  // on first call, initialize based on input arguments
  if (!pynq_log_init) {
    // if DEBUG is set, we also print log level INFO
    char const *env = getenv("DEBUG");
    if (env != NULL) {
      min_log_level = LOG_LEVEL_INFO;
    }
    // make warnings fatal
    env = getenv("FATAL_WARNING");
    if (env != NULL) {
      critical_level = LOG_LEVEL_WARNING;
    }
    pynq_log_init = true;
  }
  // check if the log level is valid
  if (level < LOG_LEVEL_INFO || level > LOG_LEVEL_ERROR) {
    printf("pynq_log: invalid log level specified (%d)\r\n", level);
    return;
  }

  if (level < min_log_level) {
    return;
  }
  fputs(color_escape_calls[level], stderr);
  fputs(log_level_name[level], stderr);

  fputs(color_escape_blue, stderr);
  if (domain != NULL) {
    fprintf(stderr, "%s::", domain);
  }
  fprintf(stderr, "%s:%d ", location, lineno);
  fputs(color_escape_reset, stderr);

  va_start(arg_list, fmt);
  vfprintf(stderr, fmt, arg_list);
  va_end(arg_list);
  if (fmt[strlen(fmt) - 1] != '\n') {
    fputs("\n", stderr);
  }

  if (level >= critical_level) {
    abort();
  }
}
