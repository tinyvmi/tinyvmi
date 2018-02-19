#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <inttypes.h>
#include <xenctrl.h>
#include "tiny_private.h"


#define DOMAIN_ID 2
#define HVM 1
#define WIDTH 4

#define KSYM_MODULES 0xc1aa1978  //for testing only, get the value from system.map.
#define KSYM_IDT_TBL 0xc1c3a000


#define MAX_ROW_LENGTH 500

/**implementation**/


status_t vmi_init(
    vmi_instance_t *vmi,
    uint32_t flags,
	char *name)
{
	status_t status = VMI_FAILURE;
	reg_t cr3=0;
	libvmi_xenctrl_handle_t m_xchandle;

	// allocate memory for instance structure //
	if(!(*vmi)){
		printf("TinyVMI Version 0.01.1-new instance\n");  //TODO change this with each release
		*vmi = (vmi_instance_t)  malloc(sizeof(struct vmi_instance));
		memset(*vmi, 0, sizeof(struct vmi_instance));
	}

	/// initialize instance struct to default values////
	printf("TinyVMI Version 0.01.1\n");  //TODO change this with each release
	
	
	printf("\ndomain id:%d\n",DOMAIN_ID);
//sleep(2);
	(*vmi)->hvm=HVM;

	printf("\ndomain id:%d\n",DOMAIN_ID);
//sleep(2);
	(*vmi)->addr_width=WIDTH;


	(*vmi)->page_shift = PAGE_SHIFT;
	(*vmi)->page_size = 1 << (*vmi)->page_shift;
	(*vmi)->page_mode = VMI_PM_PAE;
	(*vmi)->domainid = DOMAIN_ID;
	
    v2p_cache_init(*vmi);
    memory_cache_init(*vmi,0);
	
	//status_t ret = VMI_FAILURE;
	m_xchandle = XENCTRL_HANDLE_INVALID;

	/* open handle to the libxc interface */
	m_xchandle = xc_interface_open(NULL, NULL, 0);

	if (XENCTRL_HANDLE_INVALID == m_xchandle) {
		printf("Failed to open libxc interface.\n");
		return status;
	}else{
		printf("--LELE: open libxc, in %s\n",__FUNCTION__);
	}
    	(*vmi)->xchandle = m_xchandle;

	printf("--LELE: xchandle:%llx\n",(unsigned long long)(*vmi)->xchandle);
	printf("--LELE: domainid:%d\n",(*vmi)->domainid);
    
	(*vmi)->pae = 1;

	(*vmi)->os_type = VMI_OS_LINUX;

/* read CR3 and compute the kpgd address **/

	if((*vmi)->hvm){
		printf("--LELE: %s read cr3 in HVM\n",__FUNCTION__);
    		status=tiny_get_vcpureg_hvm((*vmi), &cr3,CR3,0);
	}else if((*vmi)->addr_width==32){
		printf("--LELE: %s read cr3 in PV32\n",__FUNCTION__);
		status=tiny_get_vcpureg_pv32((*vmi),&cr3,CR3,0);
  	}else if((*vmi)->addr_width==64){
		printf("--LELE: %s read cr3 in PV64\n",__FUNCTION__);
		status=tiny_get_vcpureg_pv64((*vmi),&cr3,CR3,0);
	}
  	if(status==VMI_FAILURE){
		printf("--LELE: ERROR reading cr3,in ./tiny-vmi/tiny_vmi_init.c:%s\n",__FUNCTION__);
		return status;
	}
	if(!cr3){
		printf("--LELE: ERROR: cr3=0, in ./tiny-vmi/tiny_vmi_init.c:%s\n",__FUNCTION__);
		return status;
	}
	
	//cr3=0x29b31000;

	(*vmi)->cr3=cr3;
	
	printf("--LELE: in %s, cr3= 0x%.16"PRIx64"\n",__FUNCTION__,(*vmi)->cr3);

    	(*vmi)->kpgd = cr3&0xFFFFFFE0;

	printf("--LELE: kpgd= 0x%.16"PRIx64", in %s\n",(*vmi)->kpgd,__FUNCTION__);

    	//(*vmi)->kpgd = vmi_translate_kv2p(*vmi, (*vmi)->kpgd);

	//printf("--LELE: kpgd=%lx, in%s\n",(*vmi)->kpgd,__FUNCTION__);

	printf("--LELE: VMI initial done. in %s\n",__FUNCTION__);
		
	//sleep(2);

    	status = VMI_SUCCESS;

    return status;
}


addr_t vmi_translate_ksym2v(
    vmi_instance_t vmi,
    char *symbol){

	addr_t symbol_vaddr;
	
    if (strncmp("modules", symbol, MAX_ROW_LENGTH) == 0) {
        symbol_vaddr=KSYM_MODULES;
	}else if(strncmp("modules", symbol, MAX_ROW_LENGTH) == 0) {
        symbol_vaddr=KSYM_IDT_TBL;
	}else{
		symbol_vaddr=NULL;
		printf("--%s:unknown symbol: %s",__FUNCTION__,symbol);
	}
	return symbol_vaddr;

}


