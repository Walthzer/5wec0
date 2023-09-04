#include <libpynq.h>

#include <log.h>

#undef LOG_DOMAIN
#define LOG_DOMAIN "LogTest"

int main(int argc __attribute__((unused)),
         char **argv __attribute__((unused))) {

  pynq_info("Test my info\n");
  pynq_info("Test my info arg: %s", "str arg");

  pynq_warning("Test my warning");
#undef LOG_DOMAIN
#define LOG_DOMAIN NULL
  pynq_warning("Test my warning arg: %s", "str arg\n");

  pynq_error("Test my error arg: %s", "str arg");

  return EXIT_SUCCESS;
}
