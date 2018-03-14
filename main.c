#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include "tiny_private.h"

#include "libvmi_conf_file.h"


/*
* in: char* vm_name
*
* in: char[] libvmi_conf: defined in libvmi_conf_file.h, act as 
*		string version of config file in ~/etc/libvmi.conf in the
*		original LibVMI.
* out/return: char * config: configure string for vm_name
*
*/
char * get_config_from_file_string(char *vm_name){

    FILE *f = NULL;
    const char *ptr = NULL;
    vmi_instance_t vmi = NULL;

   // /* strip path for memory image files */
    if ((ptr = strrchr(vm_name, '/')) == NULL) {
        ptr = vm_name;
    }
    else {
        ptr++;
    }

	char *buf = &libvmi_conf;	// get buf from libvmi_conf_file.h
	unsigned int sz = libvmi_conf_len; //get length from libvmi_conf_file.h

    long pos = 0;
    size_t max_len = strnlen(ptr, 100);
    int found = 0;
    for (pos = 0; pos < sz; ++pos){
        if (strncmp(buf + pos, ptr, max_len) == 0){
            found = 1;
            break;
        }
    }
    if (!found){
        errprint("failed to find config entry");
    }
    long start = pos + max_len;
    found = 0;
    for ( ; pos < sz; ++pos){
        if (buf[pos] == '}'){
            found = 1;
            break;
        }
    }
    if (!found){
        errprint("failed to find end of config entry");
    }
    long end = pos + 1;
    long entry_length = end - start;
    char *config = malloc(entry_length);
    memcpy(config, buf + start, entry_length);
 
    // free(buf);

	return config;
}


/*
* in: char* config: configure string
* out: config_ht: configure in hashtable
*/
GHashTable * get_config_hashtable(char *config){
	GHashTable *config_ht = NULL;

	return config_ht;
}


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
	char * name = "u1432hvm";

    printf("Hello, world!\n");
    // sleep(2);
	
	gettimeofday(&tv_begin,NULL);
	//printf("LELE: time stamp1: %ld\n",tv_begin.tv_usec);
    
	//result = vmi_init(&vmi,0,NULL);
	// result = vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
                        //   VMI_CONFIG_GLOBAL_FILE_ENTRY, NULL, NULL);
	result = vmi_init_complete(&vmi, name, VMI_INIT_DOMAINNAME, NULL,
                          VMI_CONFIG_STRING, get_config_from_file_string(name), NULL);

	if( VMI_FAILURE == result )
	{
		printf("vmi_init failed\n");
		goto _bail;
	}else{
		printf("--LELE:%s:vmi_initial succeed\n",__FUNCTION__);
	}
	
	printf("LELE: time stamp1: %ld\n",tv_begin.tv_usec);
	//sleep(2);
	
	gettimeofday(&tv_end,NULL);
	printf("LELE: time stamp2: %ld\n",tv_end.tv_usec);
    //sleep(2);

	duration=(tv_end.tv_sec-tv_begin.tv_sec)*1000000+(tv_end.tv_usec-tv_begin.tv_usec);
	printf("LELE: interval: (t2-t1): %dus(%dms)\n",duration,duration/1000);
    //sleep(2);

	
	//printf("--LELE: now test_v2p_pae with vaddress 0x%.16"PRIx64"\n",vaddr);
    //sleep(2/100);
	result=test_map_addr(vmi,vaddr);
	
	sleep(2);
	//result=test_module_list(vmi,modules_addr);
	sleep(1);

_bail: 

 /* cleanup any memory associated with the libvmi instance */
	if (vmi){
	   vmi_destroy(vmi);
	}

    return 0;


}
