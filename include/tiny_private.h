/* The LibVMI Library is an introspection library that simplifies access to 
 * memory in a target virtual machine or in a file containing a dump of 
 * a system's physical memory.  LibVMI is based on the XenAccess Library.
 *
 * Copyright 2011 Sandia Corporation. Under the terms of Contract
 * DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government
 * retains certain rights in this software.
 *
 * Author: Bryan D. Payne (bdpayne@acm.org)
 *
 * This file is part of LibVMI.
 *
 * LibVMI is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * LibVMI is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LibVMI.  If not, see <http://www.gnu.org/licenses/>.
 */

//#include <glib.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <inttypes.h>
#include <xenctrl.h>
#include "tiny_libvmi.h"
//#include "tiny_kv2p.h"
#include "tiny_cache.h"

//#include "tiny_GetVaOfVectorInIDT.h"

#define PAGE_SHIFT 12
#define XENCTRL_HANDLE_INVALID (-1)
//typedef int libvmi_xenctrl_handle_t;
typedef xc_interface *libvmi_xenctrl_handle_t;

/**
 * @brief LibVMI Instance.
 *
 * This struct holds all of the relavent information for an instance of
 * LibVMI.  Each time a new domain is accessed, a new instance must
 * be created using the vmi_init function.  When you are done with an instance,
 * its resources can be freed using the vmi_destroy function.
 */
struct vmi_instance {

    uint32_t domainid;                     /* add by wyq*/

    libvmi_xenctrl_handle_t xchandle;      /* add by wyq*/

    int addr_width;	    /**< address width, 4 or 8 for 32-bit/64-bit target system, 2014.11.04, lele*/

    int hvm;                /**< nonzero if HVM, 2014.11.03, lele*/

    vmi_mode_t mode;        /**< VMI_FILE, VMI_XEN, VMI_KVM */

    uint32_t flags;         /**< flags passed to init function */

    uint32_t init_mode;     /**< VMI_INIT_PARTIAL or VMI_INIT_COMPLETE */

    vmi_config_t config;    /**< configuration */

    uint32_t config_mode;     /**< VMI_CONFIG_NONE/FILE/STRING/GHASHTABLE */

    char *sysmap;           /**< system map file for domain's running kernel */

    char *image_type;       /**< image type that we are accessing */

    char *image_type_complete;  /**< full path for file images */

    uint32_t page_offset;   /**< page offset for this instance */

    uint32_t page_shift;    /**< page shift for last mapped page */

    uint32_t page_size;     /**< page size for last mapped page */

    addr_t kpgd;            /**< kernel page global directory */

    addr_t init_task;       /**< address of task struct for init */

    os_t os_type;           /**< type of os: VMI_OS_LINUX, etc */

    int pae;                /**< nonzero if PAE is enabled */

    int pse;                /**< nonzero if PSE is enabled */

    int lme;                /**< nonzero if LME is enabled */

    reg_t cr3;              /**< value in the CR3 register */

    page_mode_t page_mode;  /**< paging mode in use */

    uint64_t size;          /**< total size of target's memory */


    union {
        struct linux_instance {

            int tasks_offset;    /**< task_struct->tasks */

            int mm_offset;       /**< task_struct->mm */

            int pid_offset;      /**< task_struct->pid */

            int pgd_offset;      /**< mm_struct->pgd */

            int name_offset;     /**< task_struct->comm */
        } linux_instance;
        struct windows_instance {

            addr_t ntoskrnl;          /**< base phys address for ntoskrnl image */

            addr_t ntoskrnl_va;       /**< base virt address for ntoskrnl image */

            addr_t kdversion_block;   /**< kernel virtual address for start of KdVersionBlock structure */

            addr_t sysproc;      /**< physical address for the system process */

            int tasks_offset;    /**< EPROCESS->ActiveProcessLinks */

            int pdbase_offset;   /**< EPROCESS->Pcb.DirectoryTableBase */

            int pid_offset;      /**< EPROCESS->UniqueProcessId */

            int pname_offset;    /**< EPROCESS->ImageFileName */

            win_ver_t version;   /**< version of Windows */
        } windows_instance;
    } os;

    //GHashTable *pid_cache;  /**< hash table to hold the PID cache data */

    //GHashTable *sym_cache;  /**< hash table to hold the sym cache data */

    //GHashTable *rva_cache;  /**< hash table to hold the rva cache data */

    v2p_cache_t v2p_cache;  /**< v2p cache data */
	int v2p_cache_count;

    void *driver;           /**< driver-specific information */

	mem_cache_t memory_cache;  /**<memory cache */

	tiny_list_t memory_cache_lru;  /**< list holding the most recently used pages */

    uint32_t memory_cache_age; /**< max age of memory cache entry */

    uint32_t memory_cache_size;/**< current size of memory cache */

    uint32_t memory_cache_size_max;/**< max size of memory cache */

    unsigned int num_vcpus; /**< number of VCPUs used by this instance */

 //   GHashTable *memory_events; /**< mem event to functions mapping (key: physical address) */

 //   GHashTable *reg_events; /**< reg event to functions mapping (key: reg) */

 //   GHashTable *ss_events; /**< single step event to functions mapping (key: vcpu_id) */

    //bool shutting_down; /**< flag indicating that libvmi is shutting down */
};

/********************************
* tiny_read.c
*
*/

size_t
vmi_read_va(
    vmi_instance_t vmi,
    addr_t vaddr,
    int pid,
    void *buf,
    size_t count);

size_t
vmi_read_ksym(
    vmi_instance_t vmi,
    char *sym,
    void *buf,
    size_t count);

status_t
vmi_read_X_ksym(
    vmi_instance_t vmi,
    char *sym,
    void *value,
    int size);

status_t
vmi_read_8_ksym(
    vmi_instance_t vmi,
    char *sym,
    uint8_t * value);

status_t
vmi_read_16_ksym(
    vmi_instance_t vmi,
    char *sym,
    uint16_t * value);

status_t
vmi_read_32_ksym(
    vmi_instance_t vmi,
    char *sym,
    uint32_t * value);

status_t
vmi_read_64_ksym(
    vmi_instance_t vmi,
    char *sym,
    uint64_t * value);

status_t
vmi_read_addr_ksym(
    vmi_instance_t vmi,
    char *sym,
    addr_t *value);

//char *
//vmi_read_str_ksym(
//    vmi_instance_t vmi,
//    char *sym);

char *
vmi_read_str_va(
    vmi_instance_t vmi,
    addr_t vaddr,
    int pid);

status_t
tiny_get_vcpureg_pv64(
    vmi_instance_t vmi,
    reg_t *value,
    registers_t reg,
    unsigned long vcpu);

status_t
tiny_get_vcpureg_pv32(
    vmi_instance_t vmi,
    reg_t *value,
    registers_t reg,
    unsigned long vcpu);

status_t tiny_get_vcpureg_hvm(vmi_instance_t vmi, reg_t *value, registers_t reg, unsigned long vcpu);

status_t
tiny_get_vcpureg(
    vmi_instance_t vmi,
    reg_t *value,
    registers_t reg,
    unsigned long vcpu);

//status_t vmi_init_private(vmi_instance_t *vmi, uint32_t flags, unsigned long id, char *name, vmi_config_t *config);
//status_t tiny_init();

status_t
xen_get_vcpureg(
    vmi_instance_t vmi,
    reg_t *value,
    registers_t reg,
    unsigned long vcpu);


void tiny_print_hex(int pfn,unsigned char *data, unsigned long length);

void * tiny_get_memory_pfn(vmi_instance_t *vmi, addr_t pfn);

int test_print_physical(vmi_instance_t *vmi,int pfn);


/*
*-------------------------------------------
* tiny_kv2p.c
*
*/
addr_t v2p_nopae (vmi_instance_t vmi, addr_t dtb, addr_t vaddr);//TODO: 2014.11.13, lele
addr_t v2p_ia32e (vmi_instance_t vmi, addr_t dtb, addr_t vaddr);//TODO: 2014.11.13, lele
addr_t v2p_pae (vmi_instance_t vmi, addr_t dtb, addr_t vaddr);
uint64_t get_pdpi (vmi_instance_t instance, uint32_t vaddr, uint32_t cr3);

void *vmi_read_page(
    vmi_instance_t vmi,
    addr_t frame_num);

void *
xen_get_memory(
    vmi_instance_t vmi,
    addr_t paddr,
    uint32_t length);


void *xen_get_memory_pfn(vmi_instance_t vmi,addr_t pfn,int prot);

int vmi_get_bit(
    reg_t reg,
    int bit);
int entry_present (os_t os_type, uint64_t entry);
int page_size_flag (uint64_t entry);
uint32_t get_large_paddr (vmi_instance_t instance, uint32_t vaddr,
        uint32_t pgd_entry);

uint64_t get_pgd_pae (vmi_instance_t instance, uint32_t vaddr, uint64_t pdpe);
uint64_t get_pte_pae (vmi_instance_t instance, uint32_t vaddr, uint64_t pgd);
uint64_t get_paddr_pae (uint32_t vaddr, uint64_t pte);

int test_v2p_pae(vmi_instance_t vmi, addr_t vaddr);


/*-------------------------------------
 * cache.c
 */
 /*
    void pid_cache_init(
    vmi_instance_t vmi);
    void pid_cache_destroy(
    vmi_instance_t vmi);
    status_t pid_cache_get(
    vmi_instance_t vmi,
    int pid,
    addr_t *dtb);
    void pid_cache_set(
    vmi_instance_t vmi,
    int pid,
    addr_t dtb);
    status_t pid_cache_del(
    vmi_instance_t vmi,
    int pid);
    void pid_cache_flush(
    vmi_instance_t vmi);

    void sym_cache_init(
    vmi_instance_t vmi);
    void sym_cache_destroy(
    vmi_instance_t vmi);
    status_t sym_cache_get(
    vmi_instance_t vmi,
    addr_t base_addr,
    uint32_t pid,
    char *sym,
    addr_t *va);
    void sym_cache_set(
    vmi_instance_t vmi,
    addr_t base_addr,
    uint32_t pid,
    char *sym,
    addr_t va);
    status_t sym_cache_del(
    vmi_instance_t vmi,
    addr_t base_addr,
    uint32_t pid,
    char *sym);
    void sym_cache_flush(
    vmi_instance_t vmi);
*/

    void v2p_cache_init(
    vmi_instance_t vmi);
    void v2p_cache_destroy(
    vmi_instance_t vmi);
    status_t v2p_cache_get(
    vmi_instance_t vmi,
    addr_t va,
    addr_t dtb,
    addr_t *pa);
    void v2p_cache_set(
    vmi_instance_t vmi,
    addr_t va,
    addr_t dtb,
    addr_t pa);
    status_t v2p_cache_del(
    vmi_instance_t vmi,
    addr_t va,
    addr_t dtb);
    void v2p_cache_flush(
    vmi_instance_t vmi);

/*-----------------------------------------
 * core.c
 */



/***
 * tiny_GetVaOfVectorInIDT.h
**/

//#include <ntddk.h>
//#include <string.h>
//#include "tiny_kv2p.h"
//#include "tiny_libvmi.h"
/*typedef struct _LDR_DATA_TABLE_ENTRY
{     
  LIST_ENTRY      InLoadOrderLinks;
  LIST_ENTRY        InMemoryOrderLinks;
  LIST_ENTRY        InInitializationOrderLinks;
  PVOID                DllBase;
  PVOID                EntryPoint;
  ULONG              SizeOfImage;
  UNICODE_STRING FullDllName;
  UNICODE_STRING BaseDllName;
  ULONG               Flags;
  USHORT             LoadCount;
  USHORT             TlsIndex;
  union
  {
    LIST_ENTRY    HashLinks;
    struct
    {
      PVOID        SectionPointer;
      ULONG      CheckSum;
    };
  };
  union
  {
    ULONG            TimeDateStamp;
    PVOID           LoadedImports;
  };
  PVOID               EntryPointActivationContext;
  PVOID                PatchInformation;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;
*/
typedef unsigned char P2C_U8;
typedef unsigned short P2C_U16;
typedef unsigned long P2C_U32;
typedef short WORD;
typedef unsigned int DWORD;
#pragma pack(push,1)
typedef struct P2C_IDT_ENTRY_ {
  P2C_U16 offset_low;
  P2C_U16 selector;
  P2C_U8 reserved;
  P2C_U8 type:4;
  P2C_U8 always0:1;
  P2C_U8 dpl:2;
  P2C_U8 present:1;
  P2C_U16 offset_high;
} P2C_IDTENTRY, *PP2C_IDTENTRY;
#pragma pack(pop)
#pragma pack(push,1)
typedef struct P2C_GDT_ENTRY_ {
  P2C_U16 limit;
  P2C_U8 base1;
  P2C_U8 base2;
  P2C_U8 base3;
  P2C_U16 shuxing;
  P2C_U8 base4;
} P2C_GDTENTRY, *PP2C_GDTENTRY;
#pragma pack(pop)
#define P2C_MAKELONG(low, high) \
  ((P2C_U32)(((P2C_U16)((P2C_U32)(low) & 0xffff)) | ((P2C_U32)((P2C_U16)((P2C_U32)(high) & 0xffff))) << 16))

#define P2C_LOW16_OF_32(data) \
  ((P2C_U16)(((P2C_U32)data) & 0xffff))

#define P2C_HIGH16_OF_32(data) \
  ((P2C_U16)(((P2C_U32)data) >> 16))
#pragma pack(push,1)
typedef struct P2C_IDTR_ {
  P2C_U16 limit;    //
  P2C_U32 base;    // 
} P2C_IDTR, *PP2C_IDTR;
#pragma pack(pop)

addr_t GetVaOfVectorInIDT(vmi_instance_t vmi,int IndexInIDT);
void test_idt_vector(vmi_instance_t vmi,int IndexInIDT);


/*******
 * tiny_test.c 2014.12.01, lele
 *
*/

status_t test_map_addr(vmi_instance_t vmi, addr_t vaddr);
status_t test_module_list(vmi_instance_t vmi, addr_t vaddr);

/**********
 * tiny_xen.c
 *
 */
 
status_t xen_pause_vm(
    vmi_instance_t vmi);

status_t xen_resume_vm(
    vmi_instance_t vmi);
