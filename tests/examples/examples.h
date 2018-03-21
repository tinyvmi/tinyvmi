
#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <tiny_libvmi.h>
#include "config/libvmi_conf_file.h"

/*
* char *name, this is the VM or file that we are looking at 
* addr_t addr this is the address to map *
*/
status_t example_map_addr(char *name, addr_t addr);



/* char *name:  this is the VM or file that we are looking at */
status_t example_module_list( char *name);


status_t example_event (char *name, vmi_pid_t pid );

#endif