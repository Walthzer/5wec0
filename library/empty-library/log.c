#include <log.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static LogLevel critical_level = LOG_LEVEL_ERROR;
static LogLevel min_log_level = LOG_LEVEL_WARNING;

void pynq_log(const LogLevel level, char const *domain, char const *location,
              unsigned int lineno, char const *fmt, ...){
  va_list arg_list;
  if (level < min_log_level) {
    return;
  }
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
