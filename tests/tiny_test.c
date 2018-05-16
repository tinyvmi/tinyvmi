
#include "tiny_private.h"
#include "tiny_test.h"

#define MAX_COUNT 1000
#define NOISE_UP 100
#define NOISE_DOWN 0
#define SLEEP_INTERVAL 5

#define TEST_CACHE


// status_t test_map_addr(vmi_instance_t vmi, addr_t vaddr){
status_t test_map_addr(char *vm_name, addr_t vaddr){

	vmi_instance_t vmi=NULL;

	status_t result;
	// int IndexInIDT=1;//83;//22;

	struct timeval tv_begin,tv_end;
	long long duration, count=0, noise_ct=0;
	long int sum=0;
	long double average=0;
	int sleep_interval=0;
	long long *intervals;
	int i;

	unsigned char *memory;

	char *name = vm_name;

    /* initialize the libvmi library */

    char *config_str = get_config_from_file_string(name);

    // if (VMI_FAILURE ==
    //     vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
    //                       VMI_CONFIG_GLOBAL_FILE_ENTRY, NULL, NULL))
    if (VMI_FAILURE == 
        // vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
                        //   VMI_CONFIG_STRING, config_str, NULL))
        vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
                          VMI_CONFIG_STRING, config_str, NULL))
    {
        ttprint(VMI_TEST_MISC, "Failed to init LibVMI library.\n");
        goto error_exit;
    }

    free(config_str);

	intervals=malloc(MAX_COUNT*sizeof(long long));

	memory=malloc(PAGE_SIZE);
	

	for (;count<MAX_COUNT;count++){

		vaddr+=0x1000;
	
		ttprint(VMI_TEST_MISC, 
			"\n%s:-------- vaddress: %p ---------\n"
			,__FUNCTION__, vaddr);
		gettimeofday(&tv_begin,NULL);

		//if (PAGE_SIZE != vmi_read_va(vmi, vaddr, 0, memory, PAGE_SIZE)) {
		//		ttprint(VMI_TEST_MISC, "failed to map memory.\n");
		//		return 0;
		//	}
	
		if (VMI_FAILURE == vmi_read_va(vmi, vaddr, 0, PAGE_SIZE, memory, NULL)) {
				ttprint(VMI_TEST_MISC, "failed to map memory.\n");
				return VMI_FAILURE;
			}

		gettimeofday(&tv_end,NULL);

		ttprint(VMI_TEST_MISC, "%s: TimeStamp T1.%lld: %lld\n",
			__FUNCTION__, count,(long long)tv_begin.tv_usec);
		ttprint(VMI_TEST_MISC, "%s: TimeStamp T2.%lld: %lld\n",
			__FUNCTION__, count,(long long)tv_end.tv_usec);

		duration= (tv_end.tv_sec - tv_begin.tv_sec)*1000000 + (tv_end.tv_usec - tv_begin.tv_usec);

		sum+=duration;
		intervals[count]=duration;

		if(duration>NOISE_UP||duration<NOISE_DOWN) noise_ct++;

		average=((double)sum)/(count+1);
		ttprint(VMI_TEST_MISC, 
		"------LELE: read_va interval: (t4-t3.%lld): %lldus(average:%Lf)------\n",
		count,duration,average);

		//vmi_print_hex(memory, PAGE_SIZE);
		if(sleep_interval>SLEEP_INTERVAL) {
			//sleep(1); 
			sleep_interval=0;
		}else{
			sleep_interval++;
		}
	}

	sleep(1);
	printf("all the results of %lld times:\n",count);
	for(i=0;i<MAX_COUNT;i++){
		//printf("%ld(%d)\t\t",intervals[i],i);
		//printf("%ld\t",intervals[i]);
		printf("%lld\t%lld\n",i,intervals[i]);
	}
	printf("\n");

	average=((long double)sum)/((long double)count);

	ttprint(VMI_TEST_MISC, 
		"\n------TEST MAP_ADDRESS: average time: %Lf, total count: %lld, noise count:%lld------\n",
		average,count,noise_ct);

    //sleep(1);

	return VMI_SUCCESS;

error_exit:
	return VMI_FAILURE;

}


status_t test_module_list(char *name){//lele,2018.05.15

    vmi_instance_t vmi;
    addr_t next_module, list_head;

	struct timeval tv_begin,tv_end;
	long long duration;
	long long sum=0;
	long long average=0;

	/* initialize the libvmi library */
    char *config_str = get_config_from_file_string(name);

    if (VMI_FAILURE ==
        // vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
        //                   VMI_CONFIG_GLOBAL_FILE_ENTRY, NULL, NULL)
        vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
           VMI_CONFIG_STRING, config_str, NULL)
       )
    {
        ttprint(VMI_TEST_MISC, "Failed to init LibVMI library.\n");
        return 1;
    }

    free(config_str);

    /* pause the vm for consistent memory access */
    ttprint(VMI_TEST_MISC, "now pause vm\n");

	xen_pause_vm(vmi);

    switch(vmi_get_ostype(vmi))
    {
    case VMI_OS_LINUX:
        vmi_read_addr_ksym(vmi, "modules", &next_module);
        break;
    case VMI_OS_WINDOWS:
        vmi_read_addr_ksym(vmi, "PsLoadedModuleList", &next_module);
        break;
    default:
        goto error_exit;
    }

    ttprint(VMI_TEST_MISC, "%s: get module address: 0x%lx\n", __FUNCTION__, next_module);

    list_head = next_module;
    int count = 0;
    /* walk the module list */

	gettimeofday(&tv_begin,NULL);

    while (1) {

        /* follow the next pointer */
        addr_t tmp_next = 0;

        vmi_read_addr_va(vmi, next_module, 0, &tmp_next);

        /* if we are back at the list head, we are done */
        if (list_head == tmp_next) {
            break;
        }

        count ++;

        /* print out the module name */

        /* Note: the module struct that we are looking at has a string
         * directly following the next / prev pointers.  This is why you
         * can just add the length of 2 address fields to get the name.
         * See include/linux/module.h for mode details */
        if (VMI_OS_LINUX == vmi_get_ostype(vmi)) {
            char *modname = NULL;

            if (VMI_PM_IA32E == vmi_get_page_mode(vmi, 0)) {   // 64-bit paging
                modname = vmi_read_str_va(vmi, next_module + 16, 0);
            }
            else {
                modname = vmi_read_str_va(vmi, next_module + 8, 0);
            }
            // ttprint(VMI_TEST_MISC, "\n ---- mode name: %s -----\n", modname);
            ttprint(VMI_TEST_MISC, "\n ----module (no. %d): %s -----\n",count, modname);
            free(modname);
        }
        else if (VMI_OS_WINDOWS == vmi_get_ostype(vmi)) {

            unicode_string_t *us = NULL;

            /*
             * The offset 0x58 and 0x2c is the offset in the _LDR_DATA_TABLE_ENTRY structure
             * to the BaseDllName member.
             * These offset values are stable (at least) between XP and Windows 7.
             */

            if (VMI_PM_IA32E == vmi_get_page_mode(vmi, 0)) {
                us = vmi_read_unicode_str_va(vmi, next_module + 0x58, 0);
            } else {
                us = vmi_read_unicode_str_va(vmi, next_module + 0x2c, 0);
            }

            unicode_string_t out = { 0 };
            //         both of these work
            if (us &&
                VMI_SUCCESS == vmi_convert_str_encoding(us, &out,
                                                        "UTF-8")) {
                ttprint(VMI_TEST_MISC, "%s\n", out.contents);
                //            if (us && 
                //                VMI_SUCCESS == vmi_convert_string_encoding (us, &out, "WCHAR_T")) {
                //                printf ("%ls\n", out.contents);
                free(out.contents);
            }   // if
            if (us)
                vmi_free_unicode_str(us);
        }
        next_module = tmp_next;
        sleep(1);
    }

error_exit:

	gettimeofday(&tv_end,NULL);
	
    /* resume the vm */
    xen_resume_vm(vmi);

    /* cleanup any memory associated with the libvmi instance */
    vmi_destroy(vmi);


	sleep(1);

	ttprint(VMI_TEST_MISC, "%s: TimeStamp: %llds%lldus\n"
		,__FUNCTION__,(long long)tv_begin.tv_sec,(long long)tv_begin.tv_usec);
   
	ttprint(VMI_TEST_MISC, "%s: TimeStamp: %llds%lldus\n"
		,__FUNCTION__, (long long)tv_end.tv_sec,(long long)tv_end.tv_usec);

	duration = (tv_end.tv_sec - tv_begin.tv_sec)*1000000LL + 
			(tv_end.tv_usec - tv_begin.tv_usec);

	ttprint(VMI_TEST_MISC, 
		"------ list_module interval: %lldus------\n",duration);
	
	ttprint(VMI_TEST_MISC, "\ndone test module_list...\n");
   
	return VMI_SUCCESS;

}

