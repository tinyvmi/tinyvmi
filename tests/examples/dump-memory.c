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

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
// #include <sys/mman.h>
#include <stdio.h>

#include <tiny_config.h>
#include <tiny_libvmi.h>

#include "examples.h"

#define PAGE_SIZE 1 << 12

// int
// main(
//     int argc,
//     char **argv)
status_t dump_memory (char* vm_name, char * filename)
{
    // if ( argc != 3 )
    //     return 1;

    vmi_instance_t vmi = NULL;
    // char *filename = NULL;
    FILE *f = NULL;
    unsigned char memory[PAGE_SIZE];
    unsigned char zeros[PAGE_SIZE];

    memset(zeros, 0, PAGE_SIZE);
    addr_t address = 0;
    addr_t size = 0;
    vmi_mode_t mode;

    /* this is the VM or file that we are looking at */
    // char *name = argv[1];
    char * name = vm_name;
    status_t ret = VMI_FAILURE;

    /* this is the file name to write the memory image to */
    // filename = strndup(argv[2], 50);

    // if (VMI_FAILURE == vmi_get_access_mode(vmi, (void*)name, VMI_INIT_DOMAINNAME, NULL, &mode) )
    //     goto error_exit;

    // /* initialize the libvmi library */
    // if (VMI_FAILURE == vmi_init(&vmi, mode, (void*)name, VMI_INIT_DOMAINNAME, NULL, NULL))
    // {
    //     ttprint(VMI_TEST_MISC, "Failed to init LibVMI library.\n");
    //     goto error_exit;
    // }

    // Initialize the libvmi library.
    if (VMI_FAILURE ==
        vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
                          VMI_CONFIG_STRING, get_config_from_file_string(name), NULL))
    {
        ttprint(VMI_TEST_EVENTS, "Failed to init LibVMI library.\n");
        ret = VMI_FAILURE;
        goto bail_;
    }

    /* open the file for writing */
    /* TODO: no file system in minios, possible solutions?
    */
    if ((f = fopen(filename, "w+")) == NULL) {
        ttprint(VMI_TEST_MISC, "failed to open file for writing.\n");
        goto error_exit;
    }

    size = vmi_get_max_physical_address(vmi);

    while (address < size) {

        /* write memory to file */
        if (VMI_SUCCESS == vmi_read_pa(vmi, address, PAGE_SIZE, memory, NULL)) {
            /* memory mapped, just write to file */
            size_t written = fwrite(memory, 1, PAGE_SIZE, f);

            if (written != PAGE_SIZE) {
                ttprint(VMI_TEST_MISC, "failed to write memory to file.\n");
                goto error_exit;
            }
        }
        else {
            /* memory not mapped, write zeros to maintain offset */
            size_t written = fwrite(zeros, 1, PAGE_SIZE, f);

            if (written != PAGE_SIZE) {
                ttprint(VMI_TEST_MISC, "failed to write zeros to file.\n");
                goto error_exit;
            }
        }

        /* move on to the next page */
        address += PAGE_SIZE;
    }
    
    ret = VMI_SUCCESS;

error_exit:
    if (f)
        fclose(f);

    /* cleanup any memory associated with the libvmi instance */
    vmi_destroy(vmi);

bail_:

    return ret;
}
