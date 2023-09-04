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
#ifndef LOG_H
#define LOG_H

#define LOG_DOMAIN NULL
/**
 * @defgroup LOG Logging library
 *
 * @brief Functions for error handling and logging.
 *
 * @code
 * #include <log.h>
 *
 * int main (void)
 * {
 *    pynq_log("Print my information message");
 *    pynq_warning("Print my warning message");
 *    pynq_error("Failed on error");
 *    return EXIT_SUCCESS;
 * }
 * @endcode
 *
 *
 * Or with a custom log domain
 *
 * @code
 * #include <log.h>
 *
 * #undef LOG_DOMAIN
 * #define LOG_DOMAIN "MyApp"
 *
 * int main ( int argc, char **argv)
 * {
 *    pynq_log("Print my information message");
 *    pynq_warning("Print my warning message");
 *    pynq_error("Failed on error");
 *    return EXIT_SUCCESS;
 * }
 * @endcode
 *
 *
 * @{
 */

typedef enum LogLevel {
  /** Informational messages. */
  LOG_LEVEL_INFO,
  /** Warning messages */
  LOG_LEVEL_WARNING,
  /** Error messages */
  LOG_LEVEL_ERROR,
  /** Number of log levels */
  NUM_LOG_LEVELS
} LogLevel;

/**
 * @param level    The #LogLevel of this mssage.
 * @param domain   The log domain.
 * @param fmt      The format string.
 * @param location The location string of the message origin.
 * @param lineno   The line number of the message origin.
 * @param ...      The arguments to the format string.
 *
 * Print log messages with loglevel WARNING and higher.
 * Messages of level ERROR will result in an abort().
 *
 * Environment DEBUG  will print out level LOG_LEVEL_INFO
 * Environment FATAL_WARNING will abort after a warning.
 *
 */
void pynq_log(const LogLevel level, char const *domain, char const *location,
              unsigned int lineno, char const *fmt, ...);

/**
 * @param ...
 *
 * Wrapper around pynq_log to print info messages.
 * This expects LOG_DOMAIN to be set.
 */
#define pynq_info(...)                                                         \
  pynq_log(LOG_LEVEL_INFO, LOG_DOMAIN, __FUNCTION__, __LINE__, __VA_ARGS__)

/**
 * @param ...
 *
 * Wrapper around pynq_log to print warning messages.
 * This expects LOG_DOMAIN to be set.
 */
#define pynq_warning(...)                                                      \
  pynq_log(LOG_LEVEL_WARNING, LOG_DOMAIN, __FUNCTION__, __LINE__, __VA_ARGS__)

/**
 * @param ...
 *
 * Wrapper around pynq_log to print error messages.
 * This expects LOG_DOMAIN to be set.
 */
#define pynq_error(...)                                                        \
  do {                                                                         \
    pynq_log(LOG_LEVEL_ERROR, LOG_DOMAIN, __FUNCTION__, __LINE__,              \
             __VA_ARGS__);                                                     \
    for (;;)                                                                   \
      ;                                                                        \
  } while (0)

/** @} */
#endif // LOG_H
