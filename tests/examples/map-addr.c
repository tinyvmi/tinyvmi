/* The LibVMI Library is an introspection library that simplifies access to 
 * memory in a target virtual machine or in a file containing a dump of 
 * a system's physical memory.  LibVMI is based on the XenAccess Library.
 *
 * Copyright 2011 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
 * retains certain rights in this software.
 *
 * Author: Bryan D. Payne (bdpayne@acm.org)
 *
 * This file is part of LibVMI.
 *
 * LibVMI is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * LibVMI is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LibVMI.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <tiny_libvmi.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>

#include "examples.h"

#define PAGE_SIZE 1 << 12

/*
* char *name, this is the VM or file that we are looking at 
* addr_t addr this is the address to map *
*/
status_t map_addr(char *name, addr_t addr)
{
    
    status_t ret = VMI_FAILURE;

    vmi_instance_t vmi;

    ttprint(VMI_TEST_MISC, "now in %s\n", __FUNCTION__);
    unsigned char *memory = malloc(PAGE_SIZE);


    /* initialize the libvmi library */

    char *config_str = get_config_from_file_string(name);

    // if (VMI_FAILURE ==
    //     vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
    //                       VMI_CONFIG_GLOBAL_FILE_ENTRY, NULL, NULL))
    if (VMI_FAILURE == 
        vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
                          VMI_CONFIG_STRING, config_str, NULL))
    {
        ttprint(VMI_TEST_MISC, "Failed to init LibVMI library.\n");
        goto error_exit;
    }

    free(config_str);

    /* get the symbol's memory page */
    if (VMI_FAILURE == vmi_read_va(vmi, addr, 0, PAGE_SIZE, memory, NULL)) {
        ttprint(VMI_TEST_MISC, "failed to map memory.\n");
        goto error_exit;
    }
    ttprint(VMI_TEST_MISC, "%s: return from vmi_read_va\n", __FUNCTION__);

    vmi_print_hex(memory, PAGE_SIZE);

    ret = VMI_SUCCESS;

error_exit:
    if (memory)
        free(memory);

    /* cleanup any memory associated with the libvmi instance */
    vmi_destroy(vmi);

    return ret;
}
