#include <version.h>
/**********************
 * WARNING
 * only change the numbers in these 4 #defs; do not change anything else
 * the libpynq version in doxygen ryb.doxy is updated automatically based
 * on the next 4 lines
 **********************/
#define LIBPYNQ_RELEASE "ONCOURSE"
#define LIBPYNQ_VERSION_MAJOR 0
#define LIBPYNQ_VERSION_MINOR 2
#define LIBPYNQ_VERSION_PATCH 0
const version_t libpynq_version = {
    LIBPYNQ_RELEASE,
    LIBPYNQ_VERSION_MAJOR,
    LIBPYNQ_VERSION_MINOR,
    LIBPYNQ_VERSION_PATCH,
};
void print_version(void){};
void check_version(void){};
