#include "tiny_private.h"

/* expose virtual to physical mapping for kernel space via api call */
addr_t vmi_translate_kv2p (vmi_instance_t vmi, addr_t virt_address)
{
    reg_t cr3 = 0;

   // printf("---in %s\n",__FUNCTION__);
    if (vmi->kpgd) {
        cr3 = vmi->kpgd;
	//	printf("---has vmi->kgpd, in %s\n",__FUNCTION__);
    }
    else {
		//printf("---no vmi->kgpd in %s, read it...\n",__FUNCTION__);
        tiny_get_vcpureg(vmi, &cr3, CR3, 0);
		//printf("---no vmi->kgpd in %s, read done...\n",__FUNCTION__);
    }
    if (!cr3) {
        printf("--early bail on v2p lookup because cr3 is zero\n");
        return 0;
    }
    else {
        return vmi_pagetable_lookup(vmi, cr3, virt_address);
    }
}

addr_t vmi_pagetable_lookup (vmi_instance_t vmi, addr_t dtb, addr_t vaddr)
{
	addr_t paddr = 0;
	
		//printf("---in %s\n",__FUNCTION__);
		
		/* check if entry exists in the cachec */
		if (VMI_SUCCESS == v2p_cache_get(vmi, vaddr, dtb, &paddr)) {
	
			/* verify that address is still valid */
			uint8_t value = 0;
	
			if (VMI_SUCCESS == vmi_read_8_pa(vmi, paddr, &value)) {
				return paddr;
			}
			else {
				v2p_cache_del(vmi, vaddr, dtb);
			}
		}
	
		/* do the actual page walk in guest memory */
		if (vmi->page_mode == VMI_PM_LEGACY) {
			paddr = v2p_nopae(vmi, dtb, vaddr);
		}
		else if (vmi->page_mode == VMI_PM_PAE) {
			paddr = v2p_pae(vmi, dtb, vaddr);
		}
		else if (vmi->page_mode == VMI_PM_IA32E) {
			paddr = v2p_ia32e(vmi, dtb, vaddr);
		}
		else {
			printf("--ERROR:Invalid paging mode during vmi_pagetable_lookup, in %s\n",__FUNCTION__);
		}
	
		/* add this to the cache */
		if (paddr) {
			v2p_cache_set(vmi, vaddr, dtb, paddr);
		}
		return paddr;

}

addr_t v2p_nopae (vmi_instance_t vmi, addr_t dtb, addr_t vaddr)
{
	printf("----- function '%s' not implemented\n",__FUNCTION__);
	return NULL;
}
addr_t v2p_ia32e (vmi_instance_t vmi, addr_t dtb, addr_t vaddr)
{
	printf("----- function '%s' not implemented\n",__FUNCTION__);
	return NULL;
}


addr_t v2p_pae (vmi_instance_t vmi, addr_t dtb, addr_t vaddr)
{
    addr_t paddr = 0;
    uint64_t pdpe, pgd, pte;

    printf("--PTLookup: lookup vaddr = 0x%.16"PRIx64"\n", vaddr);
    printf("--PTLookup: dtb = 0x%.16"PRIx64"\n", dtb);
    pdpe = get_pdpi(vmi, vaddr, dtb);
    printf("--PTLookup: pdpe = 0x%.16"PRIx64"\n", pdpe);
    if (!entry_present(vmi->os_type, pdpe)) {
        return paddr;
    }
    pgd = get_pgd_pae(vmi, vaddr, pdpe);
    printf("--PTLookup: pgd = 0x%.16"PRIx64"\n", pgd);

    if (entry_present(vmi->os_type, pgd)) {
        if (page_size_flag(pgd)) {
            paddr = get_large_paddr(vmi, vaddr, pgd);
            printf("--PTLookup: 2MB page\n");
        }
        else {
            pte = get_pte_pae(vmi, vaddr, pgd);
            printf("--PTLookup: pte = 0x%.16"PRIx64"\n", pte);
            if (entry_present(vmi->os_type, pte)) {
                paddr = get_paddr_pae(vaddr, pte);
            }
        }
    }
    printf("--PTLookup: paddr = 0x%.16"PRIx64"\n", paddr);
    return paddr;
}

uint32_t get_pdptb (uint32_t pdpr)
{
    return pdpr & 0xFFFFFFE0;
}

uint32_t pdpi_index (uint32_t pdpi)
{
    return (pdpi >> 30) * sizeof(uint64_t);
}

uint64_t get_pdpi (vmi_instance_t instance, uint32_t vaddr, uint32_t cr3)
{
    uint64_t value;
    uint32_t pdpi_entry = get_pdptb(cr3) + pdpi_index(vaddr);

    printf("--PTLookup: pdpi_entry = 0x%.8x\n", pdpi_entry);
    vmi_read_64_pa(instance, pdpi_entry, &value);
    return value;
}

uint64_t pdba_base_pae (uint64_t pdpe)
{
    return pdpe & 0xFFFFFF000ULL;
}

uint32_t pgd_index (vmi_instance_t instance, uint32_t address)
{
    if (!instance->pae) {
        return (((address) >> 22) & 0x3FF) * sizeof(uint32_t);
    }
    else {
        return (((address) >> 21) & 0x1FF) * sizeof(uint64_t);
    }
}
uint64_t get_pgd_pae (vmi_instance_t instance, uint32_t vaddr, uint64_t pdpe)
{
    uint64_t value;
    uint32_t pgd_entry = pdba_base_pae(pdpe) + pgd_index(instance, vaddr);
    printf("--PTLookup: pgd_entry = 0x%.8x\n", pgd_entry);
    vmi_read_64_pa(instance, pgd_entry, &value);
    return value;
}

/* bit flag testing */
int entry_present (os_t os_type, uint64_t entry)
{
    if (vmi_get_bit(entry, 0))
        return 1;
    /* Support Windows "Transition" pages (bit 11) and not "Prototype PTE" (bit 10)
     * pages on Windows.  See http://code.google.com/p/vmitools/issues/detail?id=35
     */
    if (os_type == VMI_OS_WINDOWS
            && (vmi_get_bit(entry, 11) && !(vmi_get_bit(entry, 10))))
        return 1;
    return 0;
}

int
vmi_get_bit(
    reg_t reg,
    int bit)
{
    reg_t mask = 1 << bit;

    if (reg & mask) {
        return 1;
    }
    else {
        return 0;
    }
}

int page_size_flag (uint64_t entry)
{

    return vmi_get_bit(entry, 7);
}

uint32_t get_large_paddr (vmi_instance_t instance, uint32_t vaddr,
        uint32_t pgd_entry)
{
    if (!instance->pae) {
        return (pgd_entry & 0xFFC00000) | (vaddr & 0x3FFFFF);
    }
    else {
        return (pgd_entry & 0xFFE00000) | (vaddr & 0x1FFFFF);
    }
}

uint64_t ptba_base_pae (uint64_t pde)
{
    return pde & 0xFFFFFF000ULL;
}

/* page table */
uint32_t pte_index (vmi_instance_t instance, uint32_t address)
{
    if (!instance->pae) {
        return (((address) >> 12) & 0x3FF) * sizeof(uint32_t);
    }
    else {
        return (((address) >> 12) & 0x1FF) * sizeof(uint64_t);
    }
}

uint64_t get_pte_pae (vmi_instance_t instance, uint32_t vaddr, uint64_t pgd)
{
    uint64_t value;
    uint32_t pte_entry = ptba_base_pae(pgd) + pte_index(instance, vaddr);
    printf("--PTLookup: pte_entry = 0x%.8x\n", pte_entry);
    vmi_read_64_pa(instance, pte_entry, &value);
    return value;
}

uint64_t pte_pfn_pae (uint64_t pte)
{
    return pte & 0xFFFFFF000ULL;
}

uint64_t get_paddr_pae (uint32_t vaddr, uint64_t pte)
{
    return pte_pfn_pae(pte) | (vaddr & 0xFFF);
}




int test_v2p_pae(vmi_instance_t vmi, addr_t vaddr) {

	
	addr_t VirtualAddressOfKernelSymbol = vaddr;
	addr_t paddr = 0;

	addr_t pfn;

	void *memory;
	printf("--LELE: now in %s\n",__FUNCTION__);
	
        paddr = vmi_translate_kv2p (vmi, VirtualAddressOfKernelSymbol);

	if( 0 == paddr )
	{
		printf("translate kv2p failed!\n");
	}
	else
	{
		printf("virtaul address:0x%.16"PRIx64",physical address:0x%.16"PRIx64"\n",VirtualAddressOfKernelSymbol,paddr);
	}
	
	pfn=paddr >> vmi->page_shift;

	printf("--LELE: test pfn:0x%.16"PRIx64" for domain : %d\n",pfn,vmi->domainid);

	

//	memory=xen_get_memory_pfn(vmi,pfn,PROT_READ);

//	tiny_print_hex(pfn,memory,XC_PAGE_SIZE>>4);
	

//    printf("Hello, world!\n");

    return 0;
}
