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

#ifndef TINY_PRIVATE_H
#define TINY_PRIVATE_H

#define _GNU_SOURCE

// #ifdef HAVE_CONFIG_H
#include <tiny_config.h>
// #endif /* HAVE_CONFIG_H */

// #include <glib.h>
#include <tiny_glib.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <inttypes.h>
// #include "libvmi.h"
// #include "libvmi_extra.h"
// #include "cache.h"
#include "tiny_libvmi.h"
// #include "tiny_cache.h"
#include "cache.h"
#include "driver/tiny_memory_cache.h"

#include <events.h>
// #include "shm.h"
// #include "slat.h"
#include "rekall.h"
#include "debug.h"
#include "arch/arch_interface.h"
#include "os/os_interface.h"

#include <xenctrl.h>

//#include <console.h>  //Lele: include this will cause xen_events_48.o to have 13Gb Size

#include "domain_id.h"

#include "tiny_types.h"
//#define OS_DISABLED
// #define OS_DISABLED_Linux

// #define SYM_FILE_FROM_STRING
// #define REKALL_FILE_FROM_STRING

/* * Legacy from LibVMI 0.10.1
*/
#define PAGE_SHIFT 12
#define XENCTRL_HANDLE_INVALID (-1)
//typedef int libvmi_xenctrl_handle_t;
typedef xc_interface *libvmi_xenctrl_handle_t;

/**
 * Internal-only enumeration for various VM types.
 */
typedef enum vm_type {
    NORMAL,
    HVM,
    PV32,
    PV64
} vm_type_t;

#include "driver/driver_interface.h"


//#include "tiny_GetVaOfVectorInIDT.h"

// #define PAGE_SHIFT 12
// #define XENCTRL_HANDLE_INVALID (-1)
// //typedef int libvmi_xenctrl_handle_t;
// typedef xc_interface *libvmi_xenctrl_handle_t;

/**
 * @brief LibVMI Instance.
 *
 * This struct holds all of the relavent information for an instance of
 * LibVMI.  Each time a new domain is accessed, a new instance must
 * be created using the vmi_init function.  When you are done with an instance,
 * its resources can be freed using the vmi_destroy function.
 */
// struct vmi_instance {

//     uint32_t domainid;                     /* add by wyq*/

//     libvmi_xenctrl_handle_t xchandle;      /* add by wyq*/

//     int addr_width;	    /**< address width, 4 or 8 for 32-bit/64-bit target system, 2014.11.04, lele*/

//     int hvm;                /**< nonzero if HVM, 2014.11.03, lele*/

//     vmi_mode_t mode;        /**< VMI_FILE, VMI_XEN, VMI_KVM */

//     uint32_t flags;         /**< flags passed to init function */

//     uint32_t init_mode;     /**< VMI_INIT_PARTIAL or VMI_INIT_COMPLETE */

//     vmi_config_t config;    /**< configuration */

//     uint32_t config_mode;     /**< VMI_CONFIG_NONE/FILE/STRING/GHASHTABLE */

//     char *sysmap;           /**< system map file for domain's running kernel */

//     char *image_type;       /**< image type that we are accessing */

//     char *image_type_complete;  /**< full path for file images */

//     uint32_t page_offset;   /**< page offset for this instance */

//     uint32_t page_shift;    /**< page shift for last mapped page */

//     uint32_t page_size;     /**< page size for last mapped page */

//     addr_t kpgd;            /**< kernel page global directory */

//     addr_t init_task;       /**< address of task struct for init */

//     os_t os_type;           /**< type of os: VMI_OS_LINUX, etc */

//     int pae;                /**< nonzero if PAE is enabled */

//     int pse;                /**< nonzero if PSE is enabled */

//     int lme;                /**< nonzero if LME is enabled */

//     reg_t cr3;              /**< value in the CR3 register */

//     page_mode_t page_mode;  /**< paging mode in use */

//     uint64_t size;          /**< total size of target's memory */


//     union {
//         struct linux_instance {

//             int tasks_offset;    /**< task_struct->tasks */

//             int mm_offset;       /**< task_struct->mm */

//             int pid_offset;      /**< task_struct->pid */

//             int pgd_offset;      /**< mm_struct->pgd */

//             int name_offset;     /**< task_struct->comm */
//         } linux_instance;
//         struct windows_instance {

//             addr_t ntoskrnl;          /**< base phys address for ntoskrnl image */

//             addr_t ntoskrnl_va;       /**< base virt address for ntoskrnl image */

//             addr_t kdversion_block;   /**< kernel virtual address for start of KdVersionBlock structure */

//             addr_t sysproc;      /**< physical address for the system process */

//             int tasks_offset;    /**< EPROCESS->ActiveProcessLinks */

//             int pdbase_offset;   /**< EPROCESS->Pcb.DirectoryTableBase */

//             int pid_offset;      /**< EPROCESS->UniqueProcessId */

//             int pname_offset;    /**< EPROCESS->ImageFileName */

//             win_ver_t version;   /**< version of Windows */
//         } windows_instance;
//     } os;

//     //GHashTable *pid_cache;  /**< hash table to hold the PID cache data */

//     //GHashTable *sym_cache;  /**< hash table to hold the sym cache data */

//     //GHashTable *rva_cache;  /**< hash table to hold the rva cache data */

//     v2p_cache_t v2p_cache;  /**< v2p cache data */
// 	int v2p_cache_count;

//     void *driver;           /**< driver-specific information */

// 	mem_cache_t memory_cache;  /**<memory cache */

// 	tiny_list_t memory_cache_lru;  /**< list holding the most recently used pages */

//     uint32_t memory_cache_age; /**< max age of memory cache entry */

//     uint32_t memory_cache_size;/**< current size of memory cache */

//     uint32_t memory_cache_size_max;/**< max size of memory cache */

//     unsigned int num_vcpus; /**< number of VCPUs used by this instance */

//  //   GHashTable *memory_events; /**< mem event to functions mapping (key: physical address) */

//  //   GHashTable *reg_events; /**< reg event to functions mapping (key: reg) */

//  //   GHashTable *ss_events; /**< single step event to functions mapping (key: vcpu_id) */

//     //bool shutting_down; /**< flag indicating that libvmi is shutting down */
// };

struct vmi_instance {

    uint32_t domainid;  // lele

    vmi_mode_t mode;        /**< VMI_FILE, VMI_XEN, VMI_KVM */

    driver_interface_t driver; /**< The driver supporting the chosen mode */

    uint32_t init_flags;    /**< init flags (events, shm, etc.) */

    char *image_type;       /**< image type that we are accessing */

    char *image_type_complete;  /**< full path for file images */

    uint32_t page_shift;    /**< page shift for last mapped page */

    uint32_t page_size;     /**< page size for last mapped page */

    addr_t kpgd;            /**< kernel page global directory */

    addr_t init_task;       /**< address of task struct for init */

    union {
        struct {
            bool pse;        /**< true if PSE is enabled */

            bool transition_pages; /**< true if transition-pages are enabled */
        } x86;

        struct {
            int t0sz;           /**< TTBR0 VA size (2^(64-t0sz)) */

            int t1sz;           /**< TTBR1 VA size (2^(64-t1sz)) */

            page_size_t tg0;    /**< TTBR0 granule size: 4KB/16KB/64KB */

            page_size_t tg1;    /**< TTBR1 granule size: 4KB/16KB/64KB */
        } arm64;
    };

    page_mode_t page_mode;  /**< paging mode in use */

    arch_interface_t arch_interface; /**< architecture specific functions */

    uint64_t allocated_ram_size; /**< total size of target's allocated memory */

    addr_t max_physical_address; /**< maximum valid physical memory address + 1 */

    vm_type_t vm_type;      /**< VM type */

    os_t os_type;           /**< type of os: VMI_OS_LINUX, etc */

    os_interface_t os_interface; /**< Guest OS specific functions */

    void* os_data; /**< Guest OS specific data */

    GHashTable *pid_cache;  /**< hash table to hold the PID cache data */

    GHashTable *sym_cache;  /**< hash table to hold the sym cache data */

    GHashTable *rva_cache;  /**< hash table to hold the rva cache data */


    GHashTable *v2p_cache;  /**< hash table to hold the v2p cache data */
    // v2p_cache_t v2p_cache;  /**< v2p cache data */ //lele
	// int v2p_cache_count;

#if ENABLE_SHM_SNAPSHOT == 1
    GHashTable *v2m_cache;  /**< hash table to hold the v2m cache data */
#endif

#if ENABLE_PAGE_CACHE == 1
    GHashTable *memory_cache;  /**< hash table for memory cache */

	// mem_cache_t memory_cache;  /**LELE changed: <memory cache */

	// tiny_list_t memory_cache_lru;  /**LELE changed: double linked list holding the most recently used pages */

    GQueue *memory_cache_lru;  /**< queue holding the most recently used pages */

    uint32_t memory_cache_age; /**< max age of memory cache entry */

    //uint32_t memory_cache_size;/**LELE created: < current size of memory cache */

    uint32_t memory_cache_size_max;/**< max size of memory cache */
#else
    void *last_used_page;   /**< the last used page */

    addr_t last_used_page_key; /**< the key (addr) of the last used page */
#endif

    unsigned int num_vcpus; /**< number of VCPUs used by this instance */

#if ENABLE_XEN_EVENTS == 1 //tiny-VMI: events enabled

    vmi_event_t *guest_requested_event; /**< Handler of guest-requested events */

    vmi_event_t *cpuid_event; /**< Handler of CPUID events */

    vmi_event_t *debug_event; /**< Handler of debug exception events */

    vmi_event_t *privcall_event; /**< Handler of privileged call events */

    vmi_event_t *descriptor_access_event; /**< Handler of discriptor access events */

    GHashTable *interrupt_events; /**< interrupt event to function mapping (key: interrupt) */

    GHashTable *mem_events_on_gfn; /**< mem event to functions mapping (key: physical address) */

    GHashTable *mem_events_generic; /**< mem event to functions mapping (key: access type) */

    GHashTable *reg_events; /**< reg event to functions mapping (key: reg) */

    GHashTable *msr_events; /**< reg event to functions mapping (key: msr index) */

    GHashTable *ss_events; /**< single step event to functions mapping (key: vcpu_id) */

    GSList *step_events; /**< events to be re-registered after single-stepping them */

    uint32_t step_vcpus[MAX_SINGLESTEP_VCPUS]; /**< counter of events on vcpus for which we have internal singlestep enabled */

    gboolean event_callback; /**< flag indicating that libvmi is currently issuing an event callback */

    GHashTable *clear_events; /**< table to save vmi_clear_event requests when event_callback is set */

    GSList *swap_events; /**< list to save vmi_swap_events requests when event_callback is set */

#endif //  ENABLE_XEN_EVENTS == 1 

    gboolean shutting_down; /**< flag indicating that libvmi is shutting down */

    void *(*get_data_callback) (vmi_instance_t, addr_t, uint32_t); /**< memory_cache function */

    void (*release_data_callback) (void *, size_t); /**< memory_cache function */
};


#if ENABLE_XEN_EVENTS == 1 //tiny-VMI: events enabled

/** Event singlestep reregister wrapper */
typedef struct step_and_reg_event_wrapper {
    vmi_event_t *event;
    uint32_t vcpu_id;
    uint64_t steps;
    event_callback_t cb;
} step_and_reg_event_wrapper_t;

/** Event swap wrapper */
typedef struct swap_wrapper {
    vmi_event_t *swap_from;
    vmi_event_t *swap_to;
    vmi_event_free_t free_routine;
} swap_wrapper_t;

#endif // ENABLE_XEN_EVENTS == 1 

/** Windows' UNICODE_STRING structure (x86) */
typedef struct _windows_unicode_string32 {
    uint16_t length;
    uint16_t maximum_length;
    uint32_t pBuffer;   // pointer to string contents
} __attribute__ ((packed))
    win32_unicode_string_t;

/** Windows' UNICODE_STRING structure (x64) */
typedef struct _windows_unicode_string64 {
    uint16_t length;
    uint16_t maximum_length;
    uint32_t padding;   // align pBuffer
    uint64_t pBuffer;   // pointer to string contents
} __attribute__ ((packed))
    win64_unicode_string_t;

/*----------------------------------------------
 * Misc functions
 */
static inline
addr_t canonical_addr(addr_t va) {
    return VMI_GET_BIT(va, 47) ? (va | 0xffff000000000000) : va;
}


/*----------------------------------------------
 * convenience.c
 */
#define DBG_LINE dbprint(VMI_DEBUG_TEST, "%s: %s:%d\n", __FUNCTION__, __FILE__, __LINE__)
#define DBG_START  dbprint(VMI_DEBUG_TEST, "->-> now in %s\n", __FUNCTION__)
#define DBG_DONE dbprint(VMI_DEBUG_TEST, "<-<- done %s\n", __FUNCTION__)

#define dbprint dbprint_

#ifndef VMI_DEBUG
#define dbprint_(category, format, args...) ((void)0)
#else
    void dbprint_(
    vmi_debug_flag_t category,
    char *format,
    ...) __attribute__((format(printf,2,3)));
    // ...) __attribute__((format(printk,2,3)));
#endif

#define ttprint testprint

#ifndef VMI_TEST_PRINT
#define testprint(category, format, args...) ((void)0)
#else
    void testprint(vmi_test_print_flag_t category, char *format,
    ...) __attribute__((format(printf,2,3)));
#endif

    void errprint(
    char *format,
    ...) __attribute__((format(printf,1,2)));
    void warnprint(
    char *format,
    ...) __attribute__((format(printf,1,2)));

#define safe_malloc(size) safe_malloc_ (size, __FILE__, __LINE__)
    void *safe_malloc_(
    size_t size,
    char const *file,
    int line);
    unsigned long get_reg32(
    reg_t r);
    addr_t aligned_addr(
    vmi_instance_t vmi,
    addr_t addr);
    int is_addr_aligned(
    vmi_instance_t vmi,
    addr_t addr);

#ifdef __GNUC__
#  define UNUSED(x) UNUSED_ ## x __attribute__((__unused__))
#else
#  define UNUSED(x) UNUSED_ ## x
#endif

#ifdef __GNUC__
#  define UNUSED_FUNCTION(x) __attribute__((__unused__)) UNUSED_ ## x
#else
#  define UNUSED_FUNCTION(x) UNUSED_ ## x
#endif

/*-------------------------------------
 * accessors.c
 */
    void *vmi_read_page(
    vmi_instance_t vmi,
    addr_t frame_num);

status_t vmi_pagetable_lookup_cache(
    vmi_instance_t vmi,
    addr_t dtb,
    addr_t vaddr,
    addr_t *paddr);

/*-----------------------------------------
 * memory.c
 */

    #define PSR_MODE_BIT 0x10 // set on cpsr iff ARM32

status_t find_page_mode_live(
    vmi_instance_t vmi,
    unsigned long vcpu,
    page_mode_t *out_pm);

/*-----------------------------------------
 * strmatch.c
 */

void *boyer_moore_init(
    unsigned char *x,
    int m);
int boyer_moore2(
    void *bm,
    unsigned char *y,
    int n);
void boyer_moore_fini(
    void *bm);

int boyer_moore(
    unsigned char *x,
    int m,
    unsigned char *y,
    int n);


// /********************************
// * tiny_read.c
// *
// */

// status_t vmi_read_va(
//     vmi_instance_t vmi,
//     addr_t vaddr,
//     vmi_pid_t pid,
//     size_t count,
//     void *buf,
//     size_t *bytes_read
// );

// status_t vmi_read_ksym(
//     vmi_instance_t vmi,
//     const char *sym,
//     size_t count,
//     void *buf,
//     size_t *bytes_read
// );

// status_t
// vmi_read_X_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     void *value,
//     int size);

// status_t
// vmi_read_8_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     uint8_t * value);

// status_t
// vmi_read_16_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     uint16_t * value);

// status_t
// vmi_read_32_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     uint32_t * value);

// status_t
// vmi_read_64_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     uint64_t * value);

// status_t
// vmi_read_addr_ksym(
//     vmi_instance_t vmi,
//     char *sym,
//     addr_t *value);

// //char *
// //vmi_read_str_ksym(
// //    vmi_instance_t vmi,
// //    char *sym);

// char *
// vmi_read_str_va(
//     vmi_instance_t vmi,
//     addr_t vaddr,
//     int pid);

// status_t
// tiny_get_vcpureg_pv64(
//     vmi_instance_t vmi,
//     reg_t *value,
//     registers_t reg,
//     unsigned long vcpu);

// status_t
// tiny_get_vcpureg_pv32(
//     vmi_instance_t vmi,
//     reg_t *value,
//     registers_t reg,
//     unsigned long vcpu);

// status_t tiny_get_vcpureg_hvm(vmi_instance_t vmi, uint64_t *value, reg_t reg, unsigned long vcpu);

// status_t
// tiny_get_vcpureg(
//     vmi_instance_t vmi,
//     uint64_t *value,
//     reg_t reg,
//     unsigned long vcpu);

//status_t vmi_init_private(vmi_instance_t *vmi, uint32_t flags, unsigned long id, char *name, vmi_config_t *config);
//status_t tiny_init();

// status_t
// xen_get_vcpureg(
//     vmi_instance_t vmi,
//     reg_t *value,
//     reg_t reg,
//     unsigned long vcpu);


void tiny_print_hex(int pfn,unsigned char *data, unsigned long length);

void * tiny_get_memory_pfn(vmi_instance_t *vmi, addr_t pfn);

int test_print_physical(vmi_instance_t *vmi,int pfn);


/*
*-------------------------------------------
* tiny_kv2p.c
*
*/
// addr_t v2p_nopae (vmi_instance_t vmi, addr_t dtb, addr_t vaddr);//TODO: 2014.11.13, lele
// addr_t v2p_ia32e (vmi_instance_t vmi, addr_t dtb, addr_t vaddr);//TODO: 2014.11.13, lele
// addr_t v2p_pae (vmi_instance_t vmi, addr_t dtb, addr_t vaddr);
// uint64_t get_pdpi (vmi_instance_t instance, uint32_t vaddr, uint32_t cr3);

// void *vmi_read_page(
//     vmi_instance_t vmi,
//     addr_t frame_num);

// int vmi_get_bit(
//     reg_t reg,
//     int bit);

// int entry_present (os_t os_type, uint64_t entry);
// int page_size_flag (uint64_t entry);

// uint32_t get_large_paddr (vmi_instance_t instance, uint32_t vaddr,
        // uint32_t pgd_entry);

// uint64_t get_pgd_pae (vmi_instance_t instance, uint32_t vaddr, uint64_t pdpe);
// uint64_t get_pte_pae (vmi_instance_t instance, uint32_t vaddr, uint64_t pgd);
// uint64_t get_paddr_pae (uint32_t vaddr, uint64_t pte);

int test_v2p_pae(vmi_instance_t vmi, addr_t vaddr);




/**********
 * xen.c
 *
 */

void *
xen_get_memory(
    vmi_instance_t vmi,
    addr_t paddr,
    uint32_t length);
 
status_t xen_pause_vm(
    vmi_instance_t vmi);

status_t xen_resume_vm(
    vmi_instance_t vmi);


void *xen_get_memory_pfn(vmi_instance_t vmi,addr_t pfn,int prot);


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

    // void v2p_cache_init(
    // vmi_instance_t vmi);
    // void v2p_cache_destroy(
    // vmi_instance_t vmi);
    // status_t v2p_cache_get(
    // vmi_instance_t vmi,
    // addr_t va,
    // addr_t dtb,
    // addr_t *pa);

    // void v2p_cache_set(
    // vmi_instance_t vmi,
    // addr_t va,
    // addr_t dtb,
    // addr_t pa);
    // status_t v2p_cache_del(
    // vmi_instance_t vmi,
    // addr_t va,
    // addr_t dtb);
    // void v2p_cache_flush(
    // vmi_instance_t vmi);

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


// L331 libvmi/private.h
/*----------------------------------------------
 * events.c
 */
    status_t events_init(
        vmi_instance_t vmi);
    void events_destroy(
        vmi_instance_t vmi);
    gboolean event_entry_free (
        gpointer key,
        gpointer value,
        gpointer data);
    status_t swap_events(
        vmi_instance_t vmi,
        vmi_event_t *swap_from,
        vmi_event_t *swap_to,
        vmi_event_free_t free_routine);
    gboolean clear_events(
        gpointer key,
        gpointer value,
        gpointer data);
    gboolean clear_events_full(
        gpointer key,
        gpointer value,
        gpointer data);

    #define ghashtable_foreach(table, iter, key, val) \
        g_hash_table_iter_init(&iter, table); \
        while(g_hash_table_iter_next(&iter,(void**)key,(void**)val))

/*----------------------------------------------
 * os/windows/core.c
 */
    addr_t get_ntoskrnl_base(
        vmi_instance_t vmi,
        addr_t page_paddr);

/*----------------------------------------------
 * os/windows/kdbg.c
 */
    win_ver_t find_windows_version(
        vmi_instance_t vmi,
        addr_t kdbg);


#endif //TINY_PRIVATE_H
