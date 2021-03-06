
#ifndef EXAMPLE_H
#define EXAMPLE_H

#include <tiny_libvmi.h>
#include "config/libvmi_conf_file.h"


#define TEST_TIME_LIMIT 60  // time in seconds, used for ending while , such as in event_example.

#define MAX_COUNT_TEST 100 // iterations when testing functions iteratively, such as in test_event_example.

/**
 * accessories
*/


void print_event(vmi_event_t *event);


// TODO: file work around for dump_memory
status_t dump_memory (char* vm_name, char * filename);


status_t test_event_example(char *vm_name, vmi_pid_t pid);
status_t event_example (char *name, vmi_pid_t pid );

status_t test_interrupt_event_example(char *vm_name);
status_t interrupt_event_example (char *vm_name);

/*
* char *name, this is the VM or file that we are looking at 
* addr_t addr this is the address to map *
*/
status_t map_addr(char *name, addr_t addr);

status_t map_symbol(char *name, char *symbol);

/* char *name:  this is the VM or file that we are looking at */
status_t module_list( char *name);

status_t msr_event_example (char *vm_name);

status_t process_list (char *vm_name);

// TODO: status_t shm_snapshot_process_list()

status_t singlestep_event_example(char *vm_name);

status_t step_event_example(char *vm_name);

status_t va_pages(char *vm_name);

status_t xen_emulate_response(char *vm_name, addr_t kv_addr);

status_t monitor_event (char *name, vmi_pid_t pid );

#endif