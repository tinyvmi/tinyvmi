
#ifndef TINY_TEST_H
#define TINY_TEST_H

// #include "tiny_private.h"


// #define TEST_CONFIG_STRING config_string

// char *config_string = 
//     "ostype = \"Linux\";\n\
// 	linux_name = 0x3f0;\n\
// 	linux_tasks = 0x270;\n\
// 	linux_mm = 0x298;\n\
// 	linux_pid = 0x308;\n\
// 	linux_pgd = 0x20;\n";



/*******
 * tiny_test.c 2014.12.01, lele
 *
*/

// status_t test_map_addr(vmi_instance_t vmi, addr_t vaddr);
// status_t test_module_list(vmi_instance_t vmi, addr_t vaddr);
status_t test_map_addr(char *vm_name, addr_t vaddr);
status_t test_module_list(char *vm_name);


#endif //TINY_TEST_H

