/* The LibVMI Library is an introspection library that simplifies access to 
 * memory in a target virtual machine or in a file containing a dump of 
 * a system's physical memory.  LibVMI is based on the XenAccess Library.
 *
 * Author: Steven Maresca (steven.maresca@zentific.com)
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

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdio.h>
#include <inttypes.h>
#include <signal.h>

#include <tiny_libvmi.h>
#include <events.h>

#include "examples.h"

vmi_event_t interrupt_event;

event_response_t int3_cb(vmi_instance_t vmi, vmi_event_t *event){


    event_response_t resp = 0;

    ttprint(VMI_TEST_EVENTS, "Int 3 happened: GFN=%"PRIx64" RIP=%"PRIx64" Length: %"PRIu32"\n",
        event->interrupt_event.gfn, event->interrupt_event.gla,
        event->interrupt_event.insn_length);

    /* This callback assumes that all INT3 events are caused by
     *  a debugger or similar inside the guest, and therefore
     *  unconditionally reinjects the interrupt.
     */
    //event->interrupt_event.reinject = 1;

    // now don't reinject.
    event->interrupt_event.reinject = 0;

    /*
     * By default int3 instructions have length of 1 byte unless
     * there are prefixes attached. As adding prefixes to int3 have
     * no effect, under normal circumstances no legitimate compiler/debugger
     * would add any. However, a malicious guest could add prefixes to change
     * the instruction length. Older Xen versions (prior to 4.8) don't include this
     * information and thus this length is reported as 0. In those cases the length
     * have to be established manually, or assume a non-malicious guest as we do here.
     */
    if ( !event->interrupt_event.insn_length )
        event->interrupt_event.insn_length = 1;
    
    
    event->x86_regs->rip += event->interrupt_event.insn_length;

    resp = VMI_EVENT_RESPONSE_SET_REGISTERS; 

    ttprint(VMI_TEST_EVENTS, "%s: done.\n", __FUNCTION__);

    // return 0;
    return resp;
}

static int interrupted = 0;
static void close_handler(int sig){
    interrupted = sig;
}

status_t interrupt_event_example (char *vm_name) {

    int inter_duration = 0;
    int last_duration_start = 0;
    int count = 0;

    status_t status = VMI_SUCCESS;
	struct timeval tv_begin,tv_end;
    int duration;

    vmi_instance_t vmi;
    struct sigaction act;
    act.sa_handler = close_handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGHUP,  &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGINT,  &act, NULL);
    sigaction(SIGALRM, &act, NULL);

    char *name = NULL;

    // if(argc < 2){
    //     fprintf(stderr, "Usage: interrupt_events_example <name of VM>\n");
    //     exit(1);
    // }

    // Arg 1 is the VM name.
    // name = argv[1];
    name = vm_name;

    // Initialize the libvmi library.
    if (VMI_FAILURE ==
        vmi_init(&vmi, VMI_XEN, (void*)name, VMI_INIT_DOMAINNAME | VMI_INIT_EVENTS, NULL, NULL))
    {
        ttprint(VMI_TEST_EVENTS, "Failed to init LibVMI library.\n");
        return 1;
    }

    ttprint(VMI_TEST_EVENTS, "LibVMI init succeeded!\n");

    /* Register event to track INT3 interrupts */
    memset(&interrupt_event, 0, sizeof(vmi_event_t));
    interrupt_event.version = VMI_EVENTS_VERSION;
    interrupt_event.type = VMI_EVENT_INTERRUPT;
    interrupt_event.interrupt_event.intr = INT3;
    interrupt_event.callback = int3_cb;

    vmi_register_event(vmi, &interrupt_event);

    ttprint(VMI_TEST_EVENTS, "Waiting for events...\n");

    /* second way of clean exit */
    gettimeofday(&tv_begin,NULL);

    while(!interrupted){

        count ++;

        vmi_events_listen(vmi,500);

        if (status != VMI_SUCCESS) {
            ttprint(VMI_TEST_EVENTS, "Error waiting for events, quitting...\n");
            interrupted = -1;
        }

		gettimeofday(&tv_end,NULL);
		duration= (tv_end.tv_sec - tv_begin.tv_sec);

        if (duration > TEST_TIME_LIMIT){
            ttprint(VMI_TEST_EVENTS, "Waiting for events... timeout (%d seconds)\n", TEST_TIME_LIMIT);
            break;
        }
    }

    ttprint(VMI_TEST_EVENTS, "Finished with test.\n");

    // cleanup any memory associated with the libvmi instance
    vmi_destroy(vmi);

    return VMI_SUCCESS;
}


// status_t test_map_addr(vmi_instance_t vmi, addr_t vaddr){
status_t test_interrupt_event_example(char *vm_name){

	vmi_instance_t vmi=NULL;

	status_t result = VMI_FAILURE;
	// int IndexInIDT=1;//83;//22;

	struct timeval tv_begin,tv_end;
	long long duration, count=0, noise_ct=0;
	long int sum=0;
	long double average=0;
	int sleep_interval=0;
	long long *intervals;
	int i;

    /* initialize the libvmi library */

	intervals=malloc(MAX_COUNT_TEST*sizeof(long long));


	for (;count<MAX_COUNT_TEST;count++){

		gettimeofday(&tv_begin,NULL);
	
		if (VMI_FAILURE == interrupt_event_example(vm_name)){
				return VMI_FAILURE;
			}

		gettimeofday(&tv_end,NULL);

		// ttprint(VMI_TEST_MISC, "\n%s: TimeStamp T1.%lld: %lld\n",
		// 	__FUNCTION__, count,(long long)tv_begin.tv_usec);
		// ttprint(VMI_TEST_MISC, "%s: TimeStamp T2.%lld: %lld\n",
			// __FUNCTION__, count,(long long)tv_end.tv_usec);

		duration= (tv_end.tv_sec - tv_begin.tv_sec)*1000000 + (tv_end.tv_usec - tv_begin.tv_usec);

		sum+=duration;
		intervals[count]=duration;

		if(duration>0||duration<20000000LL) noise_ct++;

		average=((double)sum)/(count+1);
		// ttprint(VMI_TEST_MISC, 
		// "------LELE: read_va interval: (t4-t3.%lld): %lldus(average:%Lf)------\n",
		// count,duration,average);
        
	}

	sleep(1);
	printf("all the results of %lld times:\n",count);
	for(i=0;i<MAX_COUNT_TEST;i++){
		//printf("%ld(%d)\t\t",intervals[i],i);
		//printf("%ld\t",intervals[i]);
		printf("%lld\t%lld\n",i,intervals[i]);
	}
	printf("\n");

	average=((long double)sum)/((long double)count);

	ttprint(VMI_TEST_MISC, 
		"\n------TEST MAP_ADDRESS: average time: %Lf, total count: %lld, noise count:%lld------\n",
		average,count,noise_ct);

	ttprint(VMI_TEST_MISC, 
		"\n------TEST MAP_ADDRESS: total time elapsed: %lld\n",
		sum);

    //sleep(1);

    result = VMI_SUCCESS;

error_exit:

    /* cleanup any memory associated with the libvmi instance */
    // vmi_destroy(vmi);

	return result;

}

