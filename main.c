#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "tiny_private.h"
#include "tests/tiny_test.h"
#include "tests/examples/examples.h"

#include "config/libvmi_conf_file.h"

int main(void) {

    /* The following values need to be set before run this:
    * domain_name , the virtual machine name in string
	* vaddr, the virtual address to be tested, used by example_map_addr
    */
    
	char * name = "u1432hvm";

	addr_t vaddr=0xc1c3a000;


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
	//ttprint(VMI_TEST_MAIN,  "LELE: time stamp1: %ld\n",tv_begin.tv_usec);
    
	//result = vmi_init(&vmi,0,NULL);
	// result = vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
                        //   VMI_CONFIG_GLOBAL_FILE_ENTRY, NULL, NULL);
	// result = vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
    //                       VMI_CONFIG_STRING, get_config_from_file_string(name), NULL);

	// if( VMI_FAILURE == result )
	// {
	// 	ttprint(VMI_TEST_MAIN,  "vmi_init failed\n");
	// 	goto _bail;
	// }else{
	// 	ttprint(VMI_TEST_MAIN,  "--LELE:%s:vmi_initial succeed\n",__FUNCTION__);
	// }
	
	// ttprint(VMI_TEST_MAIN,  "LELE: time stamp1: %ld\n",tv_begin.tv_usec);
	//sleep(2);
	
	gettimeofday(&tv_end,NULL);
	ttprint(VMI_TEST_MAIN,  "LELE: time stamp: %ld\n",tv_end.tv_usec);
    //sleep(2);

	// duration=(tv_end.tv_sec-tv_begin.tv_sec)*1000000+(tv_end.tv_usec-tv_begin.tv_usec);
	// ttprint(VMI_TEST_MAIN,  "LELE: interval: (t2-t1): %dus(%dms)\n",duration,duration/1000);
    // //sleep(2);

	
	//ttprint(VMI_TEST_MAIN,  "--LELE: now test_v2p_pae with vaddress 0x%.16"PRIx64"\n",vaddr);
    //sleep(2/100);
	// result=test_map_addr(vmi,vaddr);
	// result=example_map_addr(name,vaddr);
	
	// sleep(2);
	// result=example_module_list(name);
	sleep(2);
	//result=event_example(name, 0);
	result=interrupt_event_example(name);
	sleep(1);

_bail: 

 /* cleanup any memory associated with the libvmi instance */
	if (vmi){
	   vmi_destroy(vmi);
	}

    return 0;


}
