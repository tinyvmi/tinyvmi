#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <inttypes.h>
#include <xenctrl.h>
#include "tiny_private.h"


#include "driver/xen/xen.h"
#include "driver/xen/xen_private.h"
// #include "driver/xen/xen_events.h"
#include "driver/driver_interface.h"
// #include "driver/memory_cache.h"
// #include "driver/xen/altp2m_private.h"


/**implementation**/

// status_t
// vmi_read_va(
//     vmi_instance_t vmi,
//     addr_t vaddr,
//     vmi_pid_t pid,
//     size_t count,
//     void *buf,
//     size_t *bytes_read)
// {
//     unsigned char *memory = NULL;
//     addr_t paddr = 0;
//     addr_t pfn = 0;
//     addr_t offset = 0;
//     size_t buf_offset = 0;

//     if (NULL == buf) {
//         dbprint(VMI_DEBUG_TEST, "--%s: buf passed as NULL, returning without read\n",
//                 __FUNCTION__);
//         return VMI_FAILURE;
//     }

//     while (count > 0) {
//         size_t read_len = 0;

//         if (pid) {
//             //paddr = vmi_translate_uv2p(vmi, vaddr + buf_offset, pid);
//             dbprint(VMI_DEBUG_TEST, "--%s: vmi_translate_uv2p not implemented\n",__FUNCTION__);
// 			return VMI_FAILURE;
//         }
//         else {
//              vmi_translate_kv2p(vmi, vaddr + buf_offset, &paddr);
//         }

//         if (!paddr) {
//             return VMI_FAILURE;
//         }

//         /* access the memory */
//         pfn = paddr >> vmi->page_shift;
//         offset = (vmi->page_size - 1) & paddr;
//         memory = vmi_read_page(vmi, pfn);
//         if (NULL == memory) {
//             return VMI_FAILURE;
//         }

//         /* determine how much we can read */
//         if ((offset + count) > vmi->page_size) {
//             read_len = vmi->page_size - offset;
//         }
//         else {
//             read_len = count;
//         }

//         /* do the read */
//         memcpy(((char *) buf) + (addr_t) buf_offset,
//                memory + (addr_t) offset, read_len);

//         /* set variables for next loop */
//         count -= read_len;
//         buf_offset += read_len;
//     }
//     *bytes_read = buf_offset;

//     return VMI_SUCCESS;
// }



// status_t
// vmi_read_ksym(
//     vmi_instance_t vmi,
//     const char *sym,
//     size_t count,
//     void *buf,
//     size_t *bytes_read)
// {

//     size_t read_bytes = 0;
//     addr_t vaddr = 0;

//     status_t ret = vmi_translate_ksym2v(vmi, sym, &vaddr);

//     if (ret == VMI_FAILURE) {
//         dbprint(VMI_DEBUG_TEST, "--%s: vmi_translate_ksym2v failed for '%s'\n",
//                 __FUNCTION__, sym);
//         return VMI_FAILURE;
//     }
//     vmi_read_va(vmi, vaddr, 0, count, buf, &read_bytes);
    
//     return VMI_SUCCESS;
// }

///////////////////////////////////////////////////////////
// Easy access to memory using kernel symbols
// status_t
// vmi_read_X_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     void *value,
//     int size)
// {
//     return vmi_read_ksym(vmi, sym, size, value, NULL);
// }

// status_t
// vmi_read_8_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     uint8_t * value)
// {
//     return vmi_read_X_ksym(vmi, sym, value, 1);
// }

// status_t
// vmi_read_16_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     uint16_t * value)
// {
//     return vmi_read_X_ksym(vmi, sym, value, 2);
// }

// status_t
// vmi_read_32_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     uint32_t * value)
// {
//     return vmi_read_X_ksym(vmi, sym, value, 4);
// }

// status_t
// vmi_read_64_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     uint64_t * value)
// {
//     return vmi_read_X_ksym(vmi, sym, value, 8);
// }

// status_t
// vmi_read_addr_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     addr_t *value)
// {
//     if (vmi->page_mode == VMI_PM_IA32E) {
//         return vmi_read_64_ksym(vmi, sym, value);
//     }
//     else {
//         uint32_t tmp = 0;
//         status_t ret = vmi_read_32_ksym(vmi, sym, &tmp);

//         *value = (uint64_t) tmp;
//         return ret;
//     }
// }

/*char *
vmi_read_str_ksym(
    vmi_instance_t vmi,
    char *sym)
{
    vmi_translate_ksym2v(vmi, sym, &vaddr);

    return vmi_read_str_va(vmi, vaddr, 0);
}
*/

///////////////////////////////////////////////////////////
// Easy access to virtual memory
// static status_t
// vmi_read_X_va(
//     vmi_instance_t vmi,
//     addr_t vaddr,
//     vmi_pid_t pid,
//     void *value,
//     size_t size)
// {
//     return vmi_read_va(vmi, vaddr, pid, size, value, NULL);

// }

// status_t
// vmi_read_8_va(
//     vmi_instance_t vmi,
//     addr_t vaddr,
//     int pid,
//     uint8_t * value)
// {
//     return vmi_read_X_va(vmi, vaddr, pid, value, 1);
// }

// status_t
// vmi_read_16_va(
//     vmi_instance_t vmi,
//     addr_t vaddr,
//     int pid,
//     uint16_t * value)
// {
//     return vmi_read_X_va(vmi, vaddr, pid, value, 2);
// }

// status_t
// vmi_read_32_va(
//     vmi_instance_t vmi,
//     addr_t vaddr,
//     int pid,
//     uint32_t * value)
// {
//     return vmi_read_X_va(vmi, vaddr, pid, value, 4);
// }

// status_t
// vmi_read_64_va(
//     vmi_instance_t vmi,
//     addr_t vaddr,
//     int pid,
//     uint64_t * value)
// {
//     return vmi_read_X_va(vmi, vaddr, pid, value, 8);
// }

// status_t
// vmi_read_addr_va(
//     vmi_instance_t vmi,
//     addr_t vaddr,
//     int pid,
//     addr_t *value)
// {
//     if (vmi->page_mode == VMI_PM_IA32E) {
//         return vmi_read_64_va(vmi, vaddr, pid, value);
//     }
//     else {
//         uint32_t tmp = 0;
//         status_t ret = vmi_read_32_va(vmi, vaddr, pid, &tmp);

//         *value = (uint64_t) tmp;
//         return ret;
//     }
// }

// char *
// vmi_read_str_va(
//     vmi_instance_t vmi,
//     addr_t vaddr,
//     int pid)
// {
//     unsigned char *memory = NULL;
//     char *rtnval = NULL;
//     addr_t paddr = 0;
//     addr_t pfn = 0;
//     addr_t offset = 0;
//     int len = 0;
//     size_t read_len = 0;
//     int read_more = 1;
 
//     rtnval = NULL;

//     status_t ret;
//     ret = VMI_FAILURE;

//     while (read_more) {
//         if (pid) {
//             //paddr = vmi_translate_uv2p(vmi, vaddr + len, pid);
//              dbprint(VMI_DEBUG_TEST, "--%s: vmi_translate_uv2p not implemented\n",__FUNCTION__);
// 			return 0;
//         }
//         else {
//             ret = vmi_translate_kv2p(vmi, vaddr + len, &paddr);
//         }

//         if (ret == VMI_FAILURE) {
//             return rtnval;
//         }

//         /* access the memory */
//         pfn = paddr >> vmi->page_shift;
//         offset = (vmi->page_size - 1) & paddr;
//         memory = vmi_read_page(vmi, pfn);
//         if (NULL == memory) {
//             return rtnval;
//         }

//         /* Count new non-null characters */
//         read_len = 0;
//         while (offset + read_len < vmi->page_size) {
//             if (memory[offset + read_len] == '\0') {
//                 read_more = 0;
//                 break;
//             }

//             read_len++;
//         }

//         /* Otherwise, realloc, tack on the '\0' in case of errors and
//          * get ready to read the next page.
//          */
//         rtnval = realloc(rtnval, len + 1 + read_len);
//         memcpy(&rtnval[len], &memory[offset], read_len);
//         len += read_len;
//         rtnval[len] = '\0';
//     }

//     return rtnval;
// }


// #if defined(I386) || defined(X86_64)

// static status_t
// tiny_get_vcpureg_pv64(
//     vmi_instance_t vmi,
//     uint64_t *value,
//     reg_t reg,
//     unsigned long vcpu)
// {
//     vcpu_guest_context_x86_64_t* vcpu_ctx = NULL;
//     vcpu_guest_context_any_t ctx;
//     xen_instance_t *xen = xen_get_instance(vmi);

//     if ( !vcpu_ctx ) {
//         if (xen->libxcw.xc_vcpu_getcontext(xen->xchandle, xen->domainid, vcpu, &ctx))
//         {
//             errprint("Failed to get context information (PV domain).\n");
//             return VMI_FAILURE;
//         }

//         vcpu_ctx = &ctx.x64;
//     }

//     switch (reg) {
//     case RAX:
//         *value = (reg_t) vcpu_ctx->user_regs.rax;
//         break;
//     case RBX:
//         *value = (reg_t) vcpu_ctx->user_regs.rbx;
//         break;
//     case RCX:
//         *value = (reg_t) vcpu_ctx->user_regs.rcx;
//         break;
//     case RDX:
//         *value = (reg_t) vcpu_ctx->user_regs.rdx;
//         break;
//     case RBP:
//         *value = (reg_t) vcpu_ctx->user_regs.rbp;
//         break;
//     case RSI:
//         *value = (reg_t) vcpu_ctx->user_regs.rsi;
//         break;
//     case RDI:
//         *value = (reg_t) vcpu_ctx->user_regs.rdi;
//         break;
//     case RSP:
//         *value = (reg_t) vcpu_ctx->user_regs.rsp;
//         break;
//     case R8:
//         *value = (reg_t) vcpu_ctx->user_regs.r8;
//         break;
//     case R9:
//         *value = (reg_t) vcpu_ctx->user_regs.r9;
//         break;
//     case R10:
//         *value = (reg_t) vcpu_ctx->user_regs.r10;
//         break;
//     case R11:
//         *value = (reg_t) vcpu_ctx->user_regs.r11;
//         break;
//     case R12:
//         *value = (reg_t) vcpu_ctx->user_regs.r12;
//         break;
//     case R13:
//         *value = (reg_t) vcpu_ctx->user_regs.r13;
//         break;
//     case R14:
//         *value = (reg_t) vcpu_ctx->user_regs.r14;
//         break;
//     case R15:
//         *value = (reg_t) vcpu_ctx->user_regs.r15;
//         break;

//     case RIP:
//         *value = (reg_t) vcpu_ctx->user_regs.rip;
//         break;
//     case RFLAGS:
//         *value = (reg_t) vcpu_ctx->user_regs.rflags;
//         break;

//     case CR0:
//         *value = (reg_t) vcpu_ctx->ctrlreg[0];
//         break;
//     case CR2:
//         *value = (reg_t) vcpu_ctx->ctrlreg[2];
//         break;
//     case CR3:
//         *value = (reg_t) vcpu_ctx->ctrlreg[3];
//         *value = (reg_t) (xen_cr3_to_pfn_x86_64(*value) << XC_PAGE_SHIFT);
//         break;
//     case CR4:
//         *value = (reg_t) vcpu_ctx->ctrlreg[4];
//         break;

//     case DR0:
//         *value = (reg_t) vcpu_ctx->debugreg[0];
//         break;
//     case DR1:
//         *value = (reg_t) vcpu_ctx->debugreg[1];
//         break;
//     case DR2:
//         *value = (reg_t) vcpu_ctx->debugreg[2];
//         break;
//     case DR3:
//         *value = (reg_t) vcpu_ctx->debugreg[3];
//         break;
//     case DR6:
//         *value = (reg_t) vcpu_ctx->debugreg[6];
//         break;
//     case DR7:
//         *value = (reg_t) vcpu_ctx->debugreg[7];
//         break;
//     case FS_BASE:
//         *value = (reg_t) vcpu_ctx->fs_base;
//         break;
//     case GS_BASE:  // TODO: distinguish between kernel & user
//         *value = (reg_t) vcpu_ctx->gs_base_kernel;
//         break;
//     case LDTR_BASE:
//         *value = (reg_t) vcpu_ctx->ldt_base;
//         break;
//     default:
//         return VMI_FAILURE;
//     }

//     return VMI_SUCCESS;
// }


// static status_t
// tiny_get_vcpureg_pv32(
//     vmi_instance_t vmi,
//     uint64_t *value,
//     reg_t reg,
//     unsigned long vcpu)
// {
//     vcpu_guest_context_x86_32_t* vcpu_ctx = NULL;
//     xen_instance_t *xen = xen_get_instance(vmi);
//     vcpu_guest_context_any_t ctx;

//     if (NULL == vcpu_ctx) {
//         if (xen->libxcw.xc_vcpu_getcontext(xen->xchandle, xen->domainid, vcpu, &ctx))
//         {
//             errprint("Failed to get context information (PV domain).\n");
//             return VMI_FAILURE;
//         }
//         vcpu_ctx = &ctx.x32;
//     }

//     switch (reg) {
//     case RAX:
//         *value = (reg_t) vcpu_ctx->user_regs.eax;
//         break;
//     case RBX:
//         *value = (reg_t) vcpu_ctx->user_regs.ebx;
//         break;
//     case RCX:
//         *value = (reg_t) vcpu_ctx->user_regs.ecx;
//         break;
//     case RDX:
//         *value = (reg_t) vcpu_ctx->user_regs.edx;
//         break;
//     case RBP:
//         *value = (reg_t) vcpu_ctx->user_regs.ebp;
//         break;
//     case RSI:
//         *value = (reg_t) vcpu_ctx->user_regs.esi;
//         break;
//     case RDI:
//         *value = (reg_t) vcpu_ctx->user_regs.edi;
//         break;
//     case RSP:
//         *value = (reg_t) vcpu_ctx->user_regs.esp;
//         break;

//     case RIP:
//         *value = (reg_t) vcpu_ctx->user_regs.eip;
//         break;
//     case RFLAGS:
//         *value = (reg_t) vcpu_ctx->user_regs.eflags;
//         break;

//     case CR0:
//         *value = (reg_t) vcpu_ctx->ctrlreg[0];
//         break;
//     case CR2:
//         *value = (reg_t) vcpu_ctx->ctrlreg[2];
//         break;
//     case CR3:
//         *value = (reg_t) vcpu_ctx->ctrlreg[3];
//         *value = (reg_t) xen_cr3_to_pfn_x86_32(*value) << XC_PAGE_SHIFT;
//         break;
//     case CR4:
//         *value = (reg_t) vcpu_ctx->ctrlreg[4];
//         break;

//     case DR0:
//         *value = (reg_t) vcpu_ctx->debugreg[0];
//         break;
//     case DR1:
//         *value = (reg_t) vcpu_ctx->debugreg[1];
//         break;
//     case DR2:
//         *value = (reg_t) vcpu_ctx->debugreg[2];
//         break;
//     case DR3:
//         *value = (reg_t) vcpu_ctx->debugreg[3];
//         break;
//     case DR6:
//         *value = (reg_t) vcpu_ctx->debugreg[6];
//         break;
//     case DR7:
//         *value = (reg_t) vcpu_ctx->debugreg[7];
//         break;
//     case LDTR_BASE:
//         *value = (reg_t) vcpu_ctx->ldt_base;
//         break;
//     default:
//         return VMI_FAILURE;
//     }

//     return VMI_SUCCESS;
// }


// #endif

// status_t
// // tiny_get_vcpureg(
//     vmi_instance_t vmi,
//     uint64_t *value,
//     reg_t reg,
//     unsigned long vcpu)
// {
// #if defined(ARM32) || defined(ARM64)
//     return xen_get_vcpureg_arm(vmi, value, reg, vcpu);
// #elif defined(I386) || defined (X86_64)
//     if (vmi->vm_type == HVM)
//         return xen_get_vcpureg_hvm (vmi, value, reg, vcpu);
//     else {
//         if (vmi->vm_type == PV64)
//             return xen_get_vcpureg_pv64(vmi, value, reg, vcpu);
//         else if (vmi->vm_type == PV32)
//             return xen_get_vcpureg_pv32(vmi, value, reg, vcpu);
//     }

//     return VMI_FAILURE;
// #endif
// }


status_t
tiny_test(
    uint64_t domainid,
    const char *name)
{
    struct vmi_instance _vmi = {0};
    vmi_instance_t vmi = &_vmi;

    dbprint(VMI_DEBUG_XEN, "->-> now in %s\n", __FUNCTION__);
  
    if (domainid == VMI_INVALID_DOMID && name == NULL) {
        errprint("VMI_ERROR: xen_test: domid or name must be specified\n");
        return VMI_FAILURE;
    }

    if ( VMI_FAILURE == xen_init(vmi, 0, NULL) )
        return VMI_FAILURE;

    if (domainid == VMI_INVALID_DOMID) { /* name != NULL */
        domainid = xen_get_domainid_from_name(vmi, name);
        if (domainid == VMI_INVALID_DOMID) {
            xen_destroy(vmi);
            return VMI_FAILURE;
        }
    }

    if ( VMI_FAILURE == xen_check_domainid(vmi, domainid) ) {
        xen_destroy(vmi);
        return VMI_FAILURE;
    }

    xen_destroy(vmi);
    return VMI_SUCCESS;
}


void tiny_print_hex(int pfn,unsigned char *data, unsigned long length)
{
    int i, j, numrows, index;
    unsigned long long address=0,base=0;
	
    numrows = (length + 15) >> 4;
	//numrows = (length) >> 4;
    base=pfn<<PAGE_SHIFT;

    for (i = 0; i < numrows; ++i) {
        /* print the byte count */
	address = base+(i*16);
        dbprint(VMI_DEBUG_TEST, "%.8llx|  ", address);

        /* print the first 8 hex values */
        for (j = 0; j < 8; ++j) {
            index = i * 16 + j;
            if (index < length) {
                dbprint(VMI_DEBUG_TEST, "%.2x ", data[index]);
            }
            else {
                dbprint(VMI_DEBUG_TEST, "   ");
            }
        }
        dbprint(VMI_DEBUG_TEST, " ");

        /* print the second 8 hex values */
        for (; j < 16; ++j) {
            index = i * 16 + j;
            if (index < length) {
                dbprint(VMI_DEBUG_TEST, "%.2x ", data[index]);
            }
            else {
                dbprint(VMI_DEBUG_TEST, "   ");
            }
        }
        dbprint(VMI_DEBUG_TEST, "  ");

        /* print the ascii values */
        for (j = 0; j < 16; ++j) {
            index = i * 16 + j;
            if (index < length) {
                if (isprint((int) data[index])) {
                    dbprint(VMI_DEBUG_TEST, "%c", data[index]);
                }
                else {
                    dbprint(VMI_DEBUG_TEST, ".");
                }
            }
        }
        dbprint(VMI_DEBUG_TEST, "\n");
    }

    	dbprint(VMI_DEBUG_TEST, "--LELE: end of function %s",__FUNCTION__);
}



/* addr_t pfn=paddr >> vmi->page_shift in libvmi/driver/xen.c:L103:xen_get_memory
	// called in get_memory_data(vmi,paddr,length)::get_data_callback(vmi,paddr,length);
	// called in create_new_entry(vmi,paddr,length) in libvmi/driver/memory_cache.c:L164
	// called in memory_cache_insert(vmi,paddr) :: create_new_entry(vmi,paddr,vmi->page_size);
	// called in xen_read_page(vmi,addr_t page) { addr_t paddr=page << vmi->page_shift; return memory_cache_insert(vmi, paddr);}
	// called in driver_read_page(vmi,page);
	// called in vmi_read_page(vmi,frame_num);
	// called in vmi_read_pa(vmi,paddr,buf,count) :: 
			//in WHILE {  phys_address = paddr + buf_offset;\
				pfn = phys_address >> vmi->page_shift;\
				offset = (vmi->page_size - 1) & phys_address;\
				memory = vmi_read_page(vmi, pfn);\
			        if ((offset + count) > vmi->page_size) {\
				    read_len = vmi->page_size - offset;}\
				else {\
				    read_len = count;\
				}\
				memcpy(((char *) buf) + (addr_t) buf_offset,\
				       memory + (addr_t) offset, read_len);\
				count -= read_len;\
				buf_offset += read_len;\
			}
	// called in vmi_read_X_pa(vmi,paddr,value,size);
	// called in vmi_read_64_pa(vmi,paddr,value) :: vmi_read_X_pa(vmi,paddr,value,8);
	// called in get_pdpi(instance,vaddr,cr3); in libvmi/memory.c:L86
	// called in v2p_pae(vmi,dtb,vaddr) :: get_padpi(vmi,vaddr,dtb); in libvmi/memory.c:L362
*/

void * tiny_get_memory_pfn(vmi_instance_t *vmi, addr_t pfn)
{
	void *memory;
	unsigned long domainid=(*vmi)->domainid;
		
    xen_instance_t *xen = xen_get_instance(vmi);
    
//libvmi_xenctrl_handle_t xchandle=NULL;

	/* open handle to the libxc interface */
//  	if (NULL == xchandle) {
//	    xchandle = xc_interface_open(NULL, NULL, 0);
//	}
//	if (NULL == xchandle) {
//	    dbprint(VMI_DEBUG_TEST, "Failed to open libxc interface.\n");
//	}
  	if (NULL == xen->xchandle) {
	    xen->xchandle = xc_interface_open(NULL, NULL, 0);
	}
	if (NULL == xen->xchandle) {
	    dbprint(VMI_DEBUG_TEST, "%s: Failed to open libxc interface.\n", __FUNCTION__);
        return NULL;
	}

	dbprint(VMI_DEBUG_TEST, "--test xchandle:%llx\n",(unsigned long long)xen->xchandle);
	dbprint(VMI_DEBUG_TEST, "--test domainid:%d\n",(*vmi)->domainid);
	memory = xc_map_foreign_range(xen->xchandle,(*vmi)->domainid,XC_PAGE_SIZE,PROT_READ,(unsigned long)pfn);
	if (MAP_FAILED == memory || NULL == memory) {
		dbprint(VMI_DEBUG_TEST, "--tiny_get_memory_pfn failed on pfn=0x%"PRIx64"\n", pfn);
		return NULL;
	    }
	else{
		dbprint(VMI_DEBUG_TEST, "--tiny_get_memory_pfn succeed!!!!!pfn=0x%"PRIx64"\n", pfn);	
	}

//#ifdef VMI_DEBUG
	// copy memory to local address space - handy for examination
//	uint8_t buf[XC_PAGE_SIZE];

//	memcpy(buf, memory, XC_PAGE_SIZE);
//#endif // VMI_DEBUG
    return memory;
//	return buf;

}

// //TODO assuming length == page size is safe for now, but isn't the most clean approach
// void *
// xen_get_memory_pfn(
//     vmi_instance_t vmi,
//     addr_t pfn,
//     int prot)
// {

//     xen_instance_t *xen = xen_get_instance(vmi);

//     void *memory = xc_map_foreign_range(xen->xchandle,
//                                         vmi->domainid,
//                                         XC_PAGE_SIZE,
//                                         prot,
//                                         (unsigned long) pfn);

//     if (NULL == memory) {
//         dbprint(VMI_DEBUG_TEST, "--xen_get_memory_pfn failed on pfn=0x%"PRIx64"\n", pfn);
//         return NULL;
//     }

// #ifdef VMI_DEBUG
//     // copy memory to local address space - handy for examination
//     uint8_t buf[XC_PAGE_SIZE];

//     memcpy(buf, memory, XC_PAGE_SIZE);
// #endif // VMI_DEBUG

//     return memory;
// }

// void *
// xen_get_memory(
//     vmi_instance_t vmi,
//     addr_t paddr,
//     uint32_t length)
// {
//     addr_t pfn = paddr >> vmi->page_shift;

//     //TODO assuming length == page size is safe for now, but isn't the most clean approach
//     return xen_get_memory_pfn(vmi, pfn, PROT_READ);
// }


///////////////////////////////////////////////////////////
// Easy access to physical memory
static status_t
vmi_read_X_pa(
    vmi_instance_t vmi,
    addr_t paddr,
    void *value,
    int size)
{
    size_t len_read = 0;
    return vmi_read_pa(vmi, paddr, size, value, &len_read);

}

status_t
vmi_read_8_pa(
    vmi_instance_t vmi,
    addr_t paddr,
    uint8_t * value)
{
    return vmi_read_X_pa(vmi, paddr, value, 1);
}
status_t
vmi_read_16_pa(
    vmi_instance_t vmi,
    addr_t paddr,
    uint16_t * value)
{
    return vmi_read_X_pa(vmi, paddr, value, 2);
}

status_t
vmi_read_32_pa(
    vmi_instance_t vmi,
    addr_t paddr,
    uint32_t * value)
{
    return vmi_read_X_pa(vmi, paddr, value, 4);
}

status_t
vmi_read_64_pa(
    vmi_instance_t vmi,
    addr_t paddr,
    uint64_t * value)
{
    return vmi_read_X_pa(vmi, paddr, value, 8);
}


// Reads memory at a guest's physical address
// size_t
// vmi_read_pa(
//     vmi_instance_t vmi,
//     addr_t paddr,
//     void *buf,
//     size_t count)

status_t vmi_read_pa(
    vmi_instance_t vmi,
    addr_t paddr,
    size_t count,
    void *buf,
    size_t *bytes_read ){
    //TODO not sure how to best handle this with respect to page size.  Is this hypervisor dependent?
    //  For example, the pfn for a given paddr should vary based on the size of the page where the
    //  paddr resides.  However, it is hard to know the page size from just the paddr.  For now, just
    //  assuming 4k pages and doing the read from there.

    unsigned char *memory = NULL;
    addr_t phys_address = 0;
    addr_t pfn = 0;
    addr_t offset = 0;
    size_t buf_offset = 0;

    dbprint(VMI_DEBUG_MEMCACHE, "->-> now in %s\n", __FUNCTION__);
    
    while (count > 0) {
        size_t read_len = 0;

        /* access the memory */
        phys_address = paddr + buf_offset;
        pfn = phys_address >> vmi->page_shift;
        offset = (vmi->page_size - 1) & phys_address;
        memory = vmi_read_page(vmi, pfn);
        if (NULL == memory) {
            return VMI_FAILURE;
        }

        /* determine how much we can read */
        if ((offset + count) > vmi->page_size) {
            read_len = vmi->page_size - offset;
        }
        else {
            read_len = count;
        }

        /* do the read */
        memcpy(((char *) buf) + (addr_t) buf_offset,
               memory + (addr_t) offset, read_len);

        /* set variables for next loop */
        count -= read_len;
        buf_offset += read_len;
    }
    *bytes_read = buf_offset;

    dbprint(VMI_DEBUG_MEMCACHE, "%s: done\n\n", __FUNCTION__);

    return VMI_SUCCESS;
}


// call routine:
//	vmi_read_page(vmi,frame_num): memory.c:L655
//	-> driver_read_page(vmi,frame_num): libvmi/driver/interface.c: L555
//	-> xen_read_page(vmi,page): libvmi/driver/xen.c:L1653
//	-> memory_cache_insert(vmi,paddr):libvmi/driver/memory_cache.c:L199
//	->
// moved to accessors.c
// void * vmi_read_page (vmi_instance_t vmi, addr_t frame_num)
// {
//     if (!frame_num) {
//         return NULL ;
//     }
//     else {
// 	//	return xen_get_memory_pfn(vmi, frame_num, PROT_READ);
//     //}

// 	addr_t paddr = frame_num << vmi->page_shift;

// 	return memory_cache_insert(vmi, paddr);
	
//     }
// }


// int test_print_physical(vmi_instance_t *vmi,int pfn){
	
// 	reg_t cr3=0;
// 	int vcpu=0;
// 	void *memory;
	
// 	dbprint(VMI_DEBUG_TEST, "--LELE: now in %s\n",__FUNCTION__);

// 	dbprint(VMI_DEBUG_TEST, "--LELE: test pfn:0x%.16"PRIx64" for domain : %d\n",pfn,(*vmi)->domainid);

// 	sleep(2);

// 	memory=tiny_get_memory_pfn(vmi,pfn);

// 	tiny_print_hex(pfn,memory,XC_PAGE_SIZE>>4);
	
// 	if(tiny_get_vcpureg_hvm((*vmi), &cr3,CR3, vcpu)==0){
// 	   dbprint(VMI_DEBUG_TEST, "--LELE: read cr3 0x%x, in domain %d, vcpu: %d\n", cr3, (*vmi)->domainid, vcpu);
// 	}
// 	else{
// 	   dbprint(VMI_DEBUG_TEST, "--LELE: ERROR calling tiny_get_vcpureg_hvm\n");
// 	}
// 	return 0;
// }


