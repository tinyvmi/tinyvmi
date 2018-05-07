
#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <tiny_libvmi.h>
#include "config/libvmi_conf_file.h"


#define TEST_TIME_LIMIT 105  // time in seconds

/*
* char *name, this is the VM or file that we are looking at 
* addr_t addr this is the address to map *
*/
status_t map_addr(char *name, addr_t addr);



/* char *name:  this is the VM or file that we are looking at */
status_t module_list( char *name);


status_t event_example (char *name, vmi_pid_t pid );

status_t interrupt_event_example (char *vm_name);

status_t map_symbol(char *name, char *symbol);

status_t monitor_event (char *name, vmi_pid_t pid );

#endif