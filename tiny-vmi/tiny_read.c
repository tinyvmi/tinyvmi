#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <inttypes.h>
#include <xenctrl.h>
#include "tiny_private.h"


/**implementation**/

size_t
vmi_read_va(
    vmi_instance_t vmi,
    addr_t vaddr,
    int pid,
    void *buf,
    size_t count)
{
    unsigned char *memory = NULL;
    addr_t paddr = 0;
    addr_t pfn = 0;
    addr_t offset = 0;
    size_t buf_offset = 0;

    if (NULL == buf) {
        printf("--%s: buf passed as NULL, returning without read\n",
                __FUNCTION__);
        return 0;
    }

    while (count > 0) {
        size_t read_len = 0;

        if (pid) {
            //paddr = vmi_translate_uv2p(vmi, vaddr + buf_offset, pid);
            printf("--%s: vmi_translate_uv2p not implemented\n",__FUNCTION__);
			return 0;
        }
        else {
            paddr = vmi_translate_kv2p(vmi, vaddr + buf_offset);
        }

        if (!paddr) {
            return buf_offset;
        }

        /* access the memory */
        pfn = paddr >> vmi->page_shift;
        offset = (vmi->page_size - 1) & paddr;
        memory = vmi_read_page(vmi, pfn);
        if (NULL == memory) {
            return buf_offset;
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

    return buf_offset;
}



size_t
vmi_read_ksym(
    vmi_instance_t vmi,
    char *sym,
    void *buf,
    size_t count)
{
    addr_t vaddr = vmi_translate_ksym2v(vmi, sym);

    if (0 == vaddr) {
        printf("--%s: vmi_translate_ksym2v failed for '%s'\n",
                __FUNCTION__, sym);
        return 0;
    }
    return vmi_read_va(vmi, vaddr, 0, buf, count);
}

///////////////////////////////////////////////////////////
// Easy access to memory using kernel symbols
status_t
vmi_read_X_ksym(
    vmi_instance_t vmi,
    char *sym,
    void *value,
    int size)
{
    size_t len_read = vmi_read_ksym(vmi, sym, value, size);

    if (len_read == size) {
        return VMI_SUCCESS;
    }
    else {
        return VMI_FAILURE;
    }
}

status_t
vmi_read_8_ksym(
    vmi_instance_t vmi,
    char *sym,
    uint8_t * value)
{
    return vmi_read_X_ksym(vmi, sym, value, 1);
}

status_t
vmi_read_16_ksym(
    vmi_instance_t vmi,
    char *sym,
    uint16_t * value)
{
    return vmi_read_X_ksym(vmi, sym, value, 2);
}

status_t
vmi_read_32_ksym(
    vmi_instance_t vmi,
    char *sym,
    uint32_t * value)
{
    return vmi_read_X_ksym(vmi, sym, value, 4);
}

status_t
vmi_read_64_ksym(
    vmi_instance_t vmi,
    char *sym,
    uint64_t * value)
{
    return vmi_read_X_ksym(vmi, sym, value, 8);
}

status_t
vmi_read_addr_ksym(
    vmi_instance_t vmi,
    char *sym,
    addr_t *value)
{
    if (vmi->page_mode == VMI_PM_IA32E) {
        return vmi_read_64_ksym(vmi, sym, value);
    }
    else {
        uint32_t tmp = 0;
        status_t ret = vmi_read_32_ksym(vmi, sym, &tmp);

        *value = (uint64_t) tmp;
        return ret;
    }
}

/*char *
vmi_read_str_ksym(
    vmi_instance_t vmi,
    char *sym)
{
    addr_t vaddr = vmi_translate_ksym2v(vmi, sym);

    return vmi_read_str_va(vmi, vaddr, 0);
}
*/

///////////////////////////////////////////////////////////
// Easy access to virtual memory
static status_t
vmi_read_X_va(
    vmi_instance_t vmi,
    addr_t vaddr,
    int pid,
    void *value,
    int size)
{
    size_t len_read = vmi_read_va(vmi, vaddr, pid, value, size);

    if (len_read == size) {
        return VMI_SUCCESS;
    }
    else {
        return VMI_FAILURE;
    }
}

status_t
vmi_read_8_va(
    vmi_instance_t vmi,
    addr_t vaddr,
    int pid,
    uint8_t * value)
{
    return vmi_read_X_va(vmi, vaddr, pid, value, 1);
}

status_t
vmi_read_16_va(
    vmi_instance_t vmi,
    addr_t vaddr,
    int pid,
    uint16_t * value)
{
    return vmi_read_X_va(vmi, vaddr, pid, value, 2);
}

status_t
vmi_read_32_va(
    vmi_instance_t vmi,
    addr_t vaddr,
    int pid,
    uint32_t * value)
{
    return vmi_read_X_va(vmi, vaddr, pid, value, 4);
}

status_t
vmi_read_64_va(
    vmi_instance_t vmi,
    addr_t vaddr,
    int pid,
    uint64_t * value)
{
    return vmi_read_X_va(vmi, vaddr, pid, value, 8);
}

status_t
vmi_read_addr_va(
    vmi_instance_t vmi,
    addr_t vaddr,
    int pid,
    addr_t *value)
{
    if (vmi->page_mode == VMI_PM_IA32E) {
        return vmi_read_64_va(vmi, vaddr, pid, value);
    }
    else {
        uint32_t tmp = 0;
        status_t ret = vmi_read_32_va(vmi, vaddr, pid, &tmp);

        *value = (uint64_t) tmp;
        return ret;
    }
}

char *
vmi_read_str_va(
    vmi_instance_t vmi,
    addr_t vaddr,
    int pid)
{
    unsigned char *memory = NULL;
    char *rtnval = NULL;
    addr_t paddr = 0;
    addr_t pfn = 0;
    addr_t offset = 0;
    int len = 0;
    size_t read_len = 0;
    int read_more = 1;
 
    rtnval = NULL;

    while (read_more) {
        if (pid) {
            //paddr = vmi_translate_uv2p(vmi, vaddr + len, pid);
             printf("--%s: vmi_translate_uv2p not implemented\n",__FUNCTION__);
			return 0;
        }
        else {
            paddr = vmi_translate_kv2p(vmi, vaddr + len);
        }

        if (!paddr) {
            return rtnval;
        }

        /* access the memory */
        pfn = paddr >> vmi->page_shift;
        offset = (vmi->page_size - 1) & paddr;
        memory = vmi_read_page(vmi, pfn);
        if (NULL == memory) {
            return rtnval;
        }

        /* Count new non-null characters */
        read_len = 0;
        while (offset + read_len < vmi->page_size) {
            if (memory[offset + read_len] == '\0') {
                read_more = 0;
                break;
            }

            read_len++;
        }

        /* Otherwise, realloc, tack on the '\0' in case of errors and
         * get ready to read the next page.
         */
        rtnval = realloc(rtnval, len + 1 + read_len);
        memcpy(&rtnval[len], &memory[offset], read_len);
        len += read_len;
        rtnval[len] = '\0';
    }

    return rtnval;
}

status_t
tiny_get_vcpureg_pv64(
    vmi_instance_t vmi,
    reg_t *value,
    registers_t reg,
    unsigned long vcpu)
{
    status_t ret = VMI_SUCCESS;
    vcpu_guest_context_any_t ctx = { 0 };
    xen_domctl_t domctl = { 0 };

    if (xc_vcpu_getcontext
        ((vmi)->xchandle, vmi->domainid, vcpu, &ctx)) {
        printf("Failed to get context information (PV domain).\n");
        ret = VMI_FAILURE;
        goto _bail;
    }

    switch (reg) {
    case RAX:
        *value = (reg_t) ctx.x64.user_regs.rax;
        break;
    case RBX:
        *value = (reg_t) ctx.x64.user_regs.rbx;
        break;
    case RCX:
        *value = (reg_t) ctx.x64.user_regs.rcx;
        break;
    case RDX:
        *value = (reg_t) ctx.x64.user_regs.rdx;
        break;
    case RBP:
        *value = (reg_t) ctx.x64.user_regs.rbp;
        break;
    case RSI:
        *value = (reg_t) ctx.x64.user_regs.rsi;
        break;
    case RDI:
        *value = (reg_t) ctx.x64.user_regs.rdi;
        break;
    case RSP:
        *value = (reg_t) ctx.x64.user_regs.rsp;
        break;
    case R8:
        *value = (reg_t) ctx.x64.user_regs.r8;
        break;
    case R9:
        *value = (reg_t) ctx.x64.user_regs.r9;
        break;
    case R10:
        *value = (reg_t) ctx.x64.user_regs.r10;
        break;
    case R11:
        *value = (reg_t) ctx.x64.user_regs.r11;
        break;
    case R12:
        *value = (reg_t) ctx.x64.user_regs.r12;
        break;
    case R13:
        *value = (reg_t) ctx.x64.user_regs.r13;
        break;
    case R14:
        *value = (reg_t) ctx.x64.user_regs.r14;
        break;
    case R15:
        *value = (reg_t) ctx.x64.user_regs.r15;
        break;

    case RIP:
        *value = (reg_t) ctx.x64.user_regs.rip;
        break;
    case RFLAGS:
        *value = (reg_t) ctx.x64.user_regs.rflags;
        break;

    case CR0:
        *value = (reg_t) ctx.x64.ctrlreg[0];
    	printf("--LELE: %s get cr0 from hw_ctxt: %x\n",__FUNCTION__,*value);
        break;
    case CR2:
        *value = (reg_t) ctx.x64.ctrlreg[2];
        break;
    case CR3:
        *value = (reg_t) ctx.x64.ctrlreg[3];
        *value = (reg_t) xen_cr3_to_pfn_x86_64(*value) << XC_PAGE_SHIFT;
    	printf("--LELE: %s get cr3 from hw_ctxt: %x\n",__FUNCTION__,*value);
        break;
    case CR4:
        *value = (reg_t) ctx.x64.ctrlreg[4];
        break;

    case DR0:
        *value = (reg_t) ctx.x64.debugreg[0];
        break;
    case DR1:
        *value = (reg_t) ctx.x64.debugreg[1];
        break;
    case DR2:
        *value = (reg_t) ctx.x64.debugreg[2];
        break;
    case DR3:
        *value = (reg_t) ctx.x64.debugreg[3];
        break;
    case DR6:
        *value = (reg_t) ctx.x64.debugreg[6];
        break;
    case DR7:
        *value = (reg_t) ctx.x64.debugreg[7];
        break;
    case FS_BASE:
        *value = (reg_t) ctx.x64.fs_base;
        break;
    case GS_BASE:  // TODO: distinguish between kernel & user
        *value = (reg_t) ctx.x64.gs_base_kernel;
        break;
    case LDTR_BASE:
        *value = (reg_t) ctx.x64.ldt_base;
        break;
    default:
        ret = VMI_FAILURE;
        break;
    }

_bail:
    return ret;
}

status_t 
tiny_get_vcpureg_pv32(
    vmi_instance_t vmi,
    reg_t *value,
    registers_t reg,
    unsigned long vcpu)
{
    status_t ret = VMI_SUCCESS;
    vcpu_guest_context_any_t ctx = { 0 };
    xen_domctl_t domctl = { 0 };
	
	if (NULL == (vmi)->xchandle) {
	    (vmi)->xchandle = xc_interface_open(NULL, NULL, 0);
	    printf("--LELE: open libxc interface.\n");
	}
	if (NULL == (vmi)->xchandle) {
	    printf("Failed to open libxc interface.\n");
	}


    if (xc_vcpu_getcontext
        (vmi->xchandle,vmi->domainid, vcpu, &ctx)) {
        printf("Failed to get context information (PV domain).\n");
        ret = VMI_FAILURE;
        goto _bail;
    }

    switch (reg) {
    case RAX:
        *value = (reg_t) ctx.x32.user_regs.eax;
        break;
    case RBX:
        *value = (reg_t) ctx.x32.user_regs.ebx;
        break;
    case RCX:
        *value = (reg_t) ctx.x32.user_regs.ecx;
        break;
    case RDX:
        *value = (reg_t) ctx.x32.user_regs.edx;
        break;
    case RBP:
        *value = (reg_t) ctx.x32.user_regs.ebp;
        break;
    case RSI:
        *value = (reg_t) ctx.x32.user_regs.esi;
        break;
    case RDI:
        *value = (reg_t) ctx.x32.user_regs.edi;
        break;
    case RSP:
        *value = (reg_t) ctx.x32.user_regs.esp;
        break;

    case RIP:
        *value = (reg_t) ctx.x32.user_regs.eip;
        break;
    case RFLAGS:
        *value = (reg_t) ctx.x32.user_regs.eflags;
        break;

    case CR0:
        *value = (reg_t) ctx.x32.ctrlreg[0];
    	printf("--LELE: %s get cr0 from hw_ctxt: %x\n",__FUNCTION__,*value);
        break;
    case CR2:
        *value = (reg_t) ctx.x32.ctrlreg[2];
        break;
    case CR3:
        *value = (reg_t) ctx.x32.ctrlreg[3];
        *value = (reg_t) xen_cr3_to_pfn_x86_32(*value) << XC_PAGE_SHIFT;
    	printf("--LELE: %s get cr3 from hw_ctxt: %x\n",__FUNCTION__,*value);
        break;
    case CR4:
        *value = (reg_t) ctx.x32.ctrlreg[4];
        break;

    case DR0:
        *value = (reg_t) ctx.x32.debugreg[0];
        break;
    case DR1:
        *value = (reg_t) ctx.x32.debugreg[1];
        break;
    case DR2:
        *value = (reg_t) ctx.x32.debugreg[2];
        break;
    case DR3:
        *value = (reg_t) ctx.x32.debugreg[3];
        break;
    case DR6:
        *value = (reg_t) ctx.x32.debugreg[6];
        break;
    case DR7:
        *value = (reg_t) ctx.x32.debugreg[7];
        break;
    case LDTR_BASE:
        *value = (reg_t) ctx.x32.ldt_base;
        break;
    default:
        ret = VMI_FAILURE;
        break;
    }

_bail:
    return ret;
}

status_t
tiny_get_vcpureg_hvm(vmi_instance_t vmi,
    reg_t *value,
    registers_t reg,
    unsigned long vcpu)
{
	
    //libvmi_xenctrl_handle_t xchandle=NULL;

    status_t ret = VMI_SUCCESS;
    struct hvm_hw_cpu hw_ctxt = { 0 };
	
	if (NULL == (vmi)->xchandle) {
	    (vmi)->xchandle = xc_interface_open(NULL, NULL, 0);
	    printf("--LELE: open libxc interface.\n");
	}
	if (NULL == (vmi)->xchandle) {
	    printf("Failed to open libxc interface.\n");
	}


    if (xc_domain_hvm_getcontext_partial
        ((vmi)->xchandle, (vmi)->domainid,
         HVM_SAVE_CODE(CPU), vcpu, &hw_ctxt, sizeof hw_ctxt) != 0) {
        printf("--LELE:ERROR: Failed to get context information (HVM domain).\n");
        ret = VMI_FAILURE;
        return ret;
    }
    printf("--LELE: get the hw_ctxt\n");

    switch (reg) {
    case RAX:
        *value = (reg_t) hw_ctxt.rax;
        break;
    case RBX:
        *value = (reg_t) hw_ctxt.rbx;
        break;
    case RCX:
        *value = (reg_t) hw_ctxt.rcx;
        break;
    case RDX:
        *value = (reg_t) hw_ctxt.rdx;
        break;
    case RBP:
        *value = (reg_t) hw_ctxt.rbp;
        break;
    case RSI:
        *value = (reg_t) hw_ctxt.rsi;
        break;
    case RDI:
        *value = (reg_t) hw_ctxt.rdi;
        break;
    case RSP:
        *value = (reg_t) hw_ctxt.rsp;
        break;
    case R8:
        *value = (reg_t) hw_ctxt.r8;
        break;
    case R9:
        *value = (reg_t) hw_ctxt.r9;
        break;
    case R10:
        *value = (reg_t) hw_ctxt.r10;
        break;
    case R11:
        *value = (reg_t) hw_ctxt.r11;
        break;
    case R12:
        *value = (reg_t) hw_ctxt.r12;
        break;
    case R13:
        *value = (reg_t) hw_ctxt.r13;
        break;
    case R14:
        *value = (reg_t) hw_ctxt.r14;
        break;
    case R15:
        *value = (reg_t) hw_ctxt.r15;
        break;
    case RIP:
        *value = (reg_t) hw_ctxt.rip;
        break;
    case RFLAGS:
        *value = (reg_t) hw_ctxt.rflags;
        break;

    case CR0:
        *value = (reg_t) hw_ctxt.cr0;
    	printf("--LELE: %s get cr0 from hw_ctxt: %x\n",__FUNCTION__,*value);
        break;
    case CR2:
        *value = (reg_t) hw_ctxt.cr2;
        break;
    case CR3:
        *value = (reg_t) hw_ctxt.cr3;
    	printf("--LELE: %s, get cr3 from hw_ctxt: %x\n",__FUNCTION__,*value);
        break;
    case CR4:
        *value = (reg_t) hw_ctxt.cr4;
        break;

    case DR0:
        *value = (reg_t) hw_ctxt.dr0;
        break;
    case DR1:
        *value = (reg_t) hw_ctxt.dr1;
        break;
    case DR2:
        *value = (reg_t) hw_ctxt.dr2;
        break;
    case DR3:
        *value = (reg_t) hw_ctxt.dr3;
        break;
    case DR6:
        *value = (reg_t) hw_ctxt.dr6;
        break;
    case DR7:
        *value = (reg_t) hw_ctxt.dr7;
        break;

    case CS_SEL:
        *value = (reg_t) hw_ctxt.cs_sel;
        break;
    case DS_SEL:
        *value = (reg_t) hw_ctxt.ds_sel;
        break;
    case ES_SEL:
        *value = (reg_t) hw_ctxt.es_sel;
        break;
    case FS_SEL:
        *value = (reg_t) hw_ctxt.fs_sel;
        break;
    case GS_SEL:
        *value = (reg_t) hw_ctxt.gs_sel;
        break;
    case SS_SEL:
        *value = (reg_t) hw_ctxt.ss_sel;
        break;
    case TR_SEL:
        *value = (reg_t) hw_ctxt.tr_sel;
        break;
    case LDTR_SEL:
        *value = (reg_t) hw_ctxt.ldtr_sel;
        break;

    case CS_LIMIT:
        *value = (reg_t) hw_ctxt.cs_limit;
        break;
    case DS_LIMIT:
        *value = (reg_t) hw_ctxt.ds_limit;
        break;
    case ES_LIMIT:
        *value = (reg_t) hw_ctxt.es_limit;
        break;
    case FS_LIMIT:
        *value = (reg_t) hw_ctxt.fs_limit;
        break;
    case GS_LIMIT:
        *value = (reg_t) hw_ctxt.gs_limit;
        break;
    case SS_LIMIT:
        *value = (reg_t) hw_ctxt.ss_limit;
        break;
    case TR_LIMIT:
        *value = (reg_t) hw_ctxt.tr_limit;
        break;
    case LDTR_LIMIT:
        *value = (reg_t) hw_ctxt.ldtr_limit;
        break;
    case IDTR_LIMIT:
        *value = (reg_t) hw_ctxt.idtr_limit;
        break;
    case GDTR_LIMIT:
        *value = (reg_t) hw_ctxt.gdtr_limit;
        break;

    case CS_BASE:
        *value = (reg_t) hw_ctxt.cs_base;
        break;
    case DS_BASE:
        *value = (reg_t) hw_ctxt.ds_base;
        break;
    case ES_BASE:
        *value = (reg_t) hw_ctxt.es_base;
        break;
    case FS_BASE:
        *value = (reg_t) hw_ctxt.fs_base;
        break;
    case GS_BASE:
        *value = (reg_t) hw_ctxt.gs_base;
        break;
    case SS_BASE:
        *value = (reg_t) hw_ctxt.ss_base;
        break;
    case TR_BASE:
        *value = (reg_t) hw_ctxt.tr_base;
        break;
    case LDTR_BASE:
        *value = (reg_t) hw_ctxt.ldtr_base;
    	printf("--LELE: %s, get LDTR_BASE from hw_ctxt: %x\n",__FUNCTION__,*value);
        break;
    case IDTR_BASE:
        *value = (reg_t) hw_ctxt.idtr_base;
    	printf("--LELE: %s, get IDTR_BASE from hw_ctxt: %x\n",__FUNCTION__,*value);
        break;
    case GDTR_BASE:
        *value = (reg_t) hw_ctxt.gdtr_base;
    	printf("--LELE: %s, get GDTR_BASE from hw_ctxt: %x\n",__FUNCTION__,*value);
        break;

    case CS_ARBYTES:
        *value = (reg_t) hw_ctxt.cs_arbytes;
        break;
    case DS_ARBYTES:
        *value = (reg_t) hw_ctxt.ds_arbytes;
        break;
    case ES_ARBYTES:
        *value = (reg_t) hw_ctxt.es_arbytes;
        break;
    case FS_ARBYTES:
        *value = (reg_t) hw_ctxt.fs_arbytes;
        break;
    case GS_ARBYTES:
        *value = (reg_t) hw_ctxt.gs_arbytes;
        break;
    case SS_ARBYTES:
        *value = (reg_t) hw_ctxt.ss_arbytes;
        break;
    case TR_ARBYTES:
        *value = (reg_t) hw_ctxt.tr_arbytes;
        break;
    case LDTR_ARBYTES:
        *value = (reg_t) hw_ctxt.ldtr_arbytes;
        break;

    case SYSENTER_CS:
        *value = (reg_t) hw_ctxt.sysenter_cs;
        break;
    case SYSENTER_ESP:
        *value = (reg_t) hw_ctxt.sysenter_esp;
        break;
    case SYSENTER_EIP:
        *value = (reg_t) hw_ctxt.sysenter_eip;
        break;
    case SHADOW_GS:
        *value = (reg_t) hw_ctxt.shadow_gs;
        break;

    case MSR_FLAGS:
        *value = (reg_t) hw_ctxt.msr_flags;
        break;
    case MSR_LSTAR:
        *value = (reg_t) hw_ctxt.msr_lstar;
        break;
    case MSR_CSTAR:
        *value = (reg_t) hw_ctxt.msr_cstar;
        break;
    case MSR_SYSCALL_MASK:
        *value = (reg_t) hw_ctxt.msr_syscall_mask;
        break;
    case MSR_EFER:
        *value = (reg_t) hw_ctxt.msr_efer;
        break;

#ifdef DECLARE_HVM_SAVE_TYPE_COMPAT
        /* Handle churn in struct hvm_hw_cpu (from xen/hvm/save.h)
         * that would prevent otherwise-compatible Xen 4.0 branches
         * from building.
         *
         * Checking this is less than ideal, but seemingly
         * the cleanest means of accomplishing the necessary check.
         *
         * see http://xenbits.xen.org/hg/xen-4.0-testing.hg/rev/57721c697c46
         */
    case MSR_TSC_AUX:
        *value = (reg_t) hw_ctxt.msr_tsc_aux;
        break;
#endif

    case TSC:
        *value = (reg_t) hw_ctxt.tsc;
        break;
    default:
        ret = VMI_FAILURE;
        break;
    }

    	printf("--LELE: end of function %s, ret=%d\n",__FUNCTION__,ret);
    return ret;
}


status_t
tiny_get_vcpureg(
    vmi_instance_t vmi,
    reg_t *value,
    registers_t reg,
    unsigned long vcpu)
{
    if (!(vmi)->hvm) {
        if (8 == vmi->addr_width) {
            return tiny_get_vcpureg_pv64(vmi, value, reg, vcpu);
        }
        else {
            return tiny_get_vcpureg_pv32(vmi, value, reg, vcpu);
        }
    }

    return tiny_get_vcpureg_hvm(vmi, value, reg, vcpu);
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
        printf("%.8llx|  ", address);

        /* print the first 8 hex values */
        for (j = 0; j < 8; ++j) {
            index = i * 16 + j;
            if (index < length) {
                printf("%.2x ", data[index]);
            }
            else {
                printf("   ");
            }
        }
        printf(" ");

        /* print the second 8 hex values */
        for (; j < 16; ++j) {
            index = i * 16 + j;
            if (index < length) {
                printf("%.2x ", data[index]);
            }
            else {
                printf("   ");
            }
        }
        printf("  ");

        /* print the ascii values */
        for (j = 0; j < 16; ++j) {
            index = i * 16 + j;
            if (index < length) {
                if (isprint((int) data[index])) {
                    printf("%c", data[index]);
                }
                else {
                    printf(".");
                }
            }
        }
        printf("\n");
    }

    	printf("--LELE: end of function %s",__FUNCTION__);
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
		
//libvmi_xenctrl_handle_t xchandle=NULL;

	/* open handle to the libxc interface */
//  	if (NULL == xchandle) {
//	    xchandle = xc_interface_open(NULL, NULL, 0);
//	}
//	if (NULL == xchandle) {
//	    printf("Failed to open libxc interface.\n");
//	}
  	if (NULL == (*vmi)->xchandle) {
	    (*vmi)->xchandle = xc_interface_open(NULL, NULL, 0);
	}
	if (NULL == (*vmi)->xchandle) {
	    printf("Failed to open libxc interface.\n");
	}

	printf("--test xchandle:%llx\n",(unsigned long long)(*vmi)->xchandle);
	printf("--test domainid:%d\n",(*vmi)->domainid);
	memory = xc_map_foreign_range((*vmi)->xchandle,(*vmi)->domainid,XC_PAGE_SIZE,PROT_READ,(unsigned long)pfn);
	if (MAP_FAILED == memory || NULL == memory) {
		printf("--tiny_get_memory_pfn failed on pfn=0x%"PRIx64"\n", pfn);
		return NULL;
	    }
	else{
		printf("--tiny_get_memory_pfn succeed!!!!!pfn=0x%"PRIx64"\n", pfn);	
	}

//#ifdef VMI_DEBUG
	// copy memory to local address space - handy for examination
//	uint8_t buf[XC_PAGE_SIZE];

//	memcpy(buf, memory, XC_PAGE_SIZE);
//#endif // VMI_DEBUG
    return memory;
//	return buf;

}

//TODO assuming length == page size is safe for now, but isn't the most clean approach
void *
xen_get_memory_pfn(
    vmi_instance_t vmi,
    addr_t pfn,
    int prot)
{
    void *memory = xc_map_foreign_range(vmi->xchandle,
                                        vmi->domainid,
                                        XC_PAGE_SIZE,
                                        prot,
                                        (unsigned long) pfn);

    if (NULL == memory) {
        printf("--xen_get_memory_pfn failed on pfn=0x%"PRIx64"\n", pfn);
        return NULL;
    }

#ifdef VMI_DEBUG
    // copy memory to local address space - handy for examination
    uint8_t buf[XC_PAGE_SIZE];

    memcpy(buf, memory, XC_PAGE_SIZE);
#endif // VMI_DEBUG

    return memory;
}

void *
xen_get_memory(
    vmi_instance_t vmi,
    addr_t paddr,
    uint32_t length)
{
    addr_t pfn = paddr >> vmi->page_shift;

    //TODO assuming length == page size is safe for now, but isn't the most clean approach
    return xen_get_memory_pfn(vmi, pfn, PROT_READ);
}


///////////////////////////////////////////////////////////
// Easy access to physical memory
static status_t
vmi_read_X_pa(
    vmi_instance_t vmi,
    addr_t paddr,
    void *value,
    int size)
{
    size_t len_read = vmi_read_pa(vmi, paddr, value, size);

    if (len_read == size) {
        return VMI_SUCCESS;
    }
    else {
        return VMI_FAILURE;
    }
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
size_t
vmi_read_pa(
    vmi_instance_t vmi,
    addr_t paddr,
    void *buf,
    size_t count)
{
    //TODO not sure how to best handle this with respect to page size.  Is this hypervisor dependent?
    //  For example, the pfn for a given paddr should vary based on the size of the page where the
    //  paddr resides.  However, it is hard to know the page size from just the paddr.  For now, just
    //  assuming 4k pages and doing the read from there.

    unsigned char *memory = NULL;
    addr_t phys_address = 0;
    addr_t pfn = 0;
    addr_t offset = 0;
    size_t buf_offset = 0;

    while (count > 0) {
        size_t read_len = 0;

        /* access the memory */
        phys_address = paddr + buf_offset;
        pfn = phys_address >> vmi->page_shift;
        offset = (vmi->page_size - 1) & phys_address;
        memory = vmi_read_page(vmi, pfn);
        if (NULL == memory) {
            return buf_offset;
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

    return buf_offset;
}


// call routine:
//	vmi_read_page(vmi,frame_num): memory.c:L655
//	-> driver_read_page(vmi,frame_num): libvmi/driver/interface.c: L555
//	-> xen_read_page(vmi,page): libvmi/driver/xen.c:L1653
//	-> memory_cache_insert(vmi,paddr):libvmi/driver/memory_cache.c:L199
//	->
void *
vmi_read_page (vmi_instance_t vmi, addr_t frame_num)
{
    if (!frame_num) {
        return NULL ;
    }
    else {
	//	return xen_get_memory_pfn(vmi, frame_num, PROT_READ);
    //}

	addr_t paddr = frame_num << vmi->page_shift;

	return memory_cache_insert(vmi, paddr);
	
    }
}


int test_print_physical(vmi_instance_t *vmi,int pfn){
	
	reg_t cr3=0;
	int vcpu=0;
	void *memory;
	
	printf("--LELE: now in %s\n",__FUNCTION__);

	printf("--LELE: test pfn:0x%.16"PRIx64" for domain : %d\n",pfn,(*vmi)->domainid);

	sleep(2);

	memory=tiny_get_memory_pfn(vmi,pfn);

	tiny_print_hex(pfn,memory,XC_PAGE_SIZE>>4);
	
	if(tiny_get_vcpureg_hvm((*vmi), &cr3,CR3, vcpu)==0){
	   printf("--LELE: read cr3 0x%x, in domain %d, vcpu: %d\n", cr3, (*vmi)->domainid, vcpu);
	}
	else{
	   printf("--LELE: ERROR calling tiny_get_vcpureg_hvm\n");
	}
	return 0;
}


