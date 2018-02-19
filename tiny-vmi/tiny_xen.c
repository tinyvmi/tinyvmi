#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <time.h>
#include <inttypes.h>
#include <xenctrl.h>
#include "tiny_private.h"


status_t 
xen_pause_vm(
    vmi_instance_t vmi)
{
    if (-1 == xc_domain_pause((vmi)->xchandle, vmi->domainid)) {
        return VMI_FAILURE;
    }
    return VMI_SUCCESS;
}

status_t
xen_resume_vm(
    vmi_instance_t vmi)
{
    if (-1 ==
        xc_domain_unpause((vmi)->xchandle, vmi->domainid)) {
        return VMI_FAILURE;
    }
    return VMI_SUCCESS;
}

