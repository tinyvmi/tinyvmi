
#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <tiny_libvmi.h>
#include "config/libvmi_conf_file.h"


#define UNIT_TEST_TIME_LIMIT 105  // time in seconds

#define UNIT_TEST_COUNT_LIMIT 1000


status_t unit_test_vmi_init (char *vm_name);

#endif