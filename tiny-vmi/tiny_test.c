
#include "tiny_private.h"

#define MAX_COUNT 10
#define NOISE_UP 100
#define NOISE_DOWN 0
#define SLEEP_INTERVAL 5

#define TEST_CACHE


status_t test_map_addr(vmi_instance_t vmi, addr_t vaddr){

	status_t result;
	int IndexInIDT=1;//83;//22;

	struct timeval tv_begin,tv_end;
	int duration,count=0,noise_ct=0;
	long int sum=0;
	long int average=0;
	int sleep_interval=0;

	unsigned char *memory=malloc(PAGE_SIZE);

	for (;count<MAX_COUNT;count++){

		vaddr+=0x1000;
	
		printf("\nLELE:---------------- vaddress: 0x%lx---------------------------------------\n",vaddr);
		gettimeofday(&tv_begin,NULL);

		printf("LELE: time stamp3.%d: %ld\n",count,tv_begin.tv_usec);

		//if (PAGE_SIZE != vmi_read_va(vmi, vaddr, 0, memory, PAGE_SIZE)) {
		//		printf("failed to map memory.\n");
		//		return 0;
		//	}
	
		if (PAGE_SIZE != vmi_read_va(vmi, vaddr, 0, memory, PAGE_SIZE)) {
				printf("failed to map memory.\n");
				return VMI_FAILURE;
			}

		gettimeofday(&tv_end,NULL);

		printf("LELE: time stamp4.%d: %d\n",count,(int)tv_end.tv_usec);

		duration= (tv_end.tv_sec - tv_begin.tv_sec)*1000000 + (tv_end.tv_usec - tv_begin.tv_usec);

		sum+=duration;
	
		if(duration>NOISE_UP||duration<NOISE_DOWN) noise_ct++;

		average=((double)sum)/(count+1);
		printf("------LELE: read_va interval: (t4-t3.%d): %dus(average:%ld)------\n",count,duration,average);

		//vmi_print_hex(memory, PAGE_SIZE);
		if(sleep_interval>SLEEP_INTERVAL) {
			//sleep(1); 
			sleep_interval=0;
		}else{
			sleep_interval++;
		}
	}

	sleep(1);
	average=(sum)/(count);

	printf("\n------TEST MAP_ADDRESS: average time: %ld, total count: %d, noise count:%d------\n",average,count,noise_ct);

    //sleep(1);

	return VMI_SUCCESS;

}


status_t test_module_list(vmi_instance_t vmi, addr_t vaddr){//lele,2014.12.11

    uint32_t offset;
    addr_t next_module, list_head;

	struct timeval tv_begin,tv_end;
	int duration,count=0,noise_ct=0;
	long int sum=0;
	long int average=0;
	int sleep_interval=0;


    /* this is the VM or file that we are looking at */
    //char *name = argv[1];

    /* pause the vm for consistent memory access */
    xen_pause_vm(vmi);

    //vmi_read_addr_ksym(vmi, "modules", &next_module);
	next_module=vaddr;
	
    list_head = next_module;

	gettimeofday(&tv_begin,NULL);

	printf("LELE: time stamp: %ds%dus\n",(int)tv_begin.tv_sec,(int)tv_begin.tv_usec);

    /* walk the module list */
    while (1) {

        /* follow the next pointer */
        addr_t tmp_next = 0;

        vmi_read_addr_va(vmi, next_module, 0, &tmp_next);

        /* if we are back at the list head, we are done */
        if (list_head == tmp_next) {
            break;
        }

        /* print out the module name */

        /* Note: the module struct that we are looking at has a string
         * directly following the next / prev pointers.  This is why you
         * can just add the length of 2 address fields to get the name.
         * See include/linux/module.h for mode details */
       // if (VMI_OS_LINUX == vmi_get_ostype(vmi)) {
            char *modname = NULL;

           // if (VMI_PM_IA32E == vmi_get_page_mode(vmi)) {   // 64-bit paging
            //    modname = vmi_read_str_va(vmi, next_module + 16, 0);
           // }
           // else {
                modname = vmi_read_str_va(vmi, next_module + 8, 0);
           // }
            printf("%s\n", modname);
            free(modname);
       
        next_module = tmp_next;
    }

	gettimeofday(&tv_end,NULL);
	
    /* resume the vm */
    xen_resume_vm(vmi);

	sleep(1);
	printf("LELE: time stamp: %ds%dus\n",(int)tv_end.tv_sec,(int)tv_end.tv_usec);

	duration= (tv_end.tv_sec - tv_begin.tv_sec)*1000000 + (tv_end.tv_usec - tv_begin.tv_usec);

	printf("------LELE: list_module interval: %dus------\n",duration);
	
	printf("\ndone test module_list...\n");
   
	return VMI_SUCCESS;

}

