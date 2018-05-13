#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "tiny_private.h"
#include "tests/tiny_test.h"
#include "tests/examples/examples.h"

#include "config/libvmi_conf_file.h"

int main(void) {

	// DOMAIN_NAME is defined in include/domain_id.h
	char *name = DOMAIN_NAME;

	addr_t vaddr=0xc1c3a000;

	addr_t kvaddr = vaddr;

	// int pfn= 
	// 	0x1c19000;
	// int IndexInIDT=1;//83;//22;

		//0xffffffffc0475008;
	// addr_t modules_addr=
	// 	//0xffffffff81c5db90;// modules addr from system.map
	// 	0xc1aa1978;

	vmi_instance_t vmi=0;

	status_t result;

	struct timeval tv_begin,tv_end;
	int duration,count=0,noise_ct=0;
	long int sum=0;
	long int average=0;
	int sleep_interval=0;

    ttprint(VMI_TEST_MAIN,  "Hello, world!\n");
    // sleep(2);
	
	gettimeofday(&tv_begin,NULL);
	ttprint(VMI_TEST_MAIN,  "LELE: time stamp1: %ld\n",tv_begin.tv_usec);
  
	//sleep(2);

	result = 
		// map_addr(name,vaddr);
		// map_symbol(name, "startup_32"); //"xen_entry"
		// module_list(name);

		event_example(name, 0);
		// interrupt_event_example(name);

		// 6 newly added, 05122018:
		// msr_event_example(name);
		// process_list(name);
		// singlestep_event_example(name);
		// step_event_example(name);
		// va_pages(name);
		// xen_emulate_response(name, kvaddr);
		
	
	// gettimeofday(&tv_end,NULL);
	// ttprint(VMI_TEST_MAIN,  "LELE: time stamp: %ld\n",tv_end.tv_usec);
    // //sleep(2);

	// duration=(tv_end.tv_sec-tv_begin.tv_sec)*1000000+(tv_end.tv_usec-tv_begin.tv_usec);
	// ttprint(VMI_TEST_MAIN,  "LELE: interval: (t2-t1): %dus(%dms)\n",duration,duration/1000);
    // //sleep(2);
	
	//ttprint(VMI_TEST_MAIN,  "--LELE: now test_v2p_pae with vaddress 0x%.16"PRIx64"\n",vaddr);
	
	sleep(2);
	sleep(1);

_bail: 

 /* cleanup any memory associated with the libvmi instance */
	if (vmi){
	   vmi_destroy(vmi);
	}

    return 0;


}
