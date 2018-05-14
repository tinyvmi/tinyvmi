
#include "libvmi_conf_file.h"
//#include "./target_conf/target_libvmi_conf_file.h"

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
        errprint("%s: failed to find config entry\n", __FUNCTION__);
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
    char *config = malloc(entry_length + 1);
    // memcpy(config, buf + start, entry_length);
    snprintf(config, entry_length + 1, "%s", buf+start);
 
    // free(buf);

	return config;
}


/*
* in: char* config: configure string
* out: config_ht: configure in hashtable
*/
GHashTable * get_config_hashtable_from_string(char *config){
	GHashTable *config_ht = NULL;
    
	return config_ht;
}
