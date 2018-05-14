/* The LibVMI Library is an introspection library that simplifies access to
 * memory in a target virtual machine or in a file containing a dump of
 * a system's physical memory.  LibVMI is based on the XenAccess Library.
 *
 * Copyright 2011 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
 * retains certain rights in this software.
 *
 * Author: Lele Ma (lelema.cn@gmail.com)
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
#include <stdio.h>

#include <tiny_config.h>
#include <tiny_libvmi.h>

#include "unit_tests.h"

#define PAGE_SIZE 1 << 12

static inline status_t _unit_test_vmi_init_ (char* vm_name)
{

    vmi_instance_t vmi = NULL;

    /* this is the VM or file that we are looking at */
    // char *name = argv[1];
    char * name = vm_name;
    status_t ret = VMI_FAILURE;

    // Initialize the libvmi library.

    char *config_str = get_config_from_file_string(name);

    if (VMI_FAILURE ==
        vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
                          VMI_CONFIG_STRING, config_str, NULL))
    {
        DBG_LINE;            
        ttprint(VMI_TEST_EVENTS, "Failed to init LibVMI library.\n");
        ret = VMI_FAILURE;
        goto bail_;
    }
    
    free(config_str);

    /* cleanup any memory associated with the libvmi instance */
    ret = vmi_destroy(vmi);

bail_:

    return ret;
}


status_t unit_test_vmi_init (char *vm_name){

    status_t ret;

    int count_max = UNIT_TEST_COUNT_LIMIT;

    struct timeval tv_begin,tv_end;
	int duration,count=0,noise_ct=0;
	long int sum=0;
	double average=0.0;
	int sleep_interval=0;

	gettimeofday(&tv_begin,NULL);
    
	ttprint(VMI_TEST_MISC, "%s: TimeStamp: %d s %d us\n", __FUNCTION__, (int)tv_begin.tv_sec,(int)tv_begin.tv_usec);

    ret = VMI_SUCCESS;
    while (count < count_max && ret == VMI_SUCCESS){
        ret = _unit_test_vmi_init_(vm_name);
        count ++;
    }

    if (ret == VMI_FAILURE){
		DBG_LINE;
		errprint("%s: VMI INIT failed, count = %d\n", __FUNCTION__, count);
    }

	gettimeofday(&tv_end,NULL);

	ttprint(VMI_TEST_MISC, "%s: TimeStamp: %d s %d us\n", __FUNCTION__, (int)tv_end.tv_sec,(int)tv_end.tv_usec);

	duration= (tv_end.tv_sec - tv_begin.tv_sec)*1000000 + (tv_end.tv_usec - tv_begin.tv_usec);

    average = (double) duration / (double) count_max;

	ttprint(VMI_TEST_MISC, "%s: TotalTime: %d us------\n",__FUNCTION__, duration);

	ttprint(VMI_TEST_MISC, "%s: Average: %f us------\n",__FUNCTION__, average);

    return VMI_SUCCESS;

}