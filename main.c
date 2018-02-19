#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "tiny_private.h"

int main(void) {

    /* The following values need to be set before run this:
    * domainid -- the id of target domain being monitored
    * pfn -- the physical frame number to be tested.
    * vaddr -- the virtual machine address to be tested.
    * for testing, pfn and vaddr can be borrowed from libvmi's introspection logs. 
    * 
    */
    
	int pfn= 
		0x1c19000;

	addr_t vaddr=0xc1c3a000;
		//0xffffffffc0475008;
	addr_t modules_addr=
		//0xffffffff81c5db90;// modules addr from system.map
		0xc1aa1978;

	vmi_instance_t vmi=0;

	status_t result;
	int IndexInIDT=1;//83;//22;

	struct timeval tv_begin,tv_end;
	int duration,count=0,noise_ct=0;
	long int sum=0;
	long int average=0;
	int sleep_interval=0;

    printf("Hello, world!\n");
    // sleep(2);
	
	gettimeofday(&tv_begin,NULL);
	//printf("LELE: time stamp1: %ld\n",tv_begin.tv_usec);
    
	result = vmi_init(&vmi,0,NULL);
	printf("LELE: time stamp1: %ld\n",tv_begin.tv_usec);
    //sleep(2);
	
	gettimeofday(&tv_end,NULL);
	printf("LELE: time stamp2: %ld\n",tv_end.tv_usec);
    //sleep(2);

	duration=(tv_end.tv_sec-tv_begin.tv_sec)*1000000+(tv_end.tv_usec-tv_begin.tv_usec);
	printf("LELE: interval: (t2-t1): %dus(%dms)\n",duration,duration/1000);
    //sleep(2);

	if( VMI_FAILURE == result )
	{
		printf("vmi_init failed\n");
	}else{
		printf("--LELE:%s:vmi_initial succeed\n",__FUNCTION__);
	}
	
	//printf("--LELE: now test_v2p_pae with vaddress 0x%.16"PRIx64"\n",vaddr);
    //sleep(2/100);
	//result=test_map_addr(vmi,vaddr);
	
	sleep(2);
	result=test_module_list(vmi,modules_addr);
	sleep(1);

 /* cleanup any memory associated with the libvmi instance */
  //  vmi_destroy(vmi);

    return 0;


}
