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

	status_t result;

	struct timeval tv_begin,tv_end;

	int duration;
    ttprint(VMI_TEST_MAIN,  "Hello, world!\n");
  
	//sleep(2);
	
	gettimeofday(&tv_begin,NULL);

	result = 
		/* examples in libvmi */
		// map_addr(name,vaddr);
		// map_symbol(name, "startup_32"); //"xen_entry"
		// module_list(name);
		// event_example(name, 0);
		// interrupt_event_example(name);
		// 6 newly added, 05122018:
		// msr_event_example(name);
		// process_list(name);
		// singlestep_event_example(name);
		// step_event_example(name);
		// va_pages(name);
		// xen_emulate_response(name, kvaddr);

		/* unit tests*/
		unit_test_vmi_init(name);
	
	if (result == VMI_FAILURE){
		DBG_LINE;
		errprint("%s: VMI Failure\n", __FUNCTION__);
	}
	gettimeofday(&tv_end,NULL);
    // //sleep(2);

	duration=(tv_end.tv_sec-tv_begin.tv_sec)*1000000+(tv_end.tv_usec-tv_begin.tv_usec);
	ttprint(VMI_TEST_MAIN,  "%s: interval: (t2-t1): %dus(%dms)\n",duration,duration/1000);
	
    return 0;

}
