#ifndef DOMAIN_ID_H
#define DOMAIN_ID_H

/* The following values need to be set before run TinyVMI:
* domain_name , the virtual machine name in string
*/

//#define DOMAIN_NAME "u1432hvm" 
// #define DOMAIN_NAME "ubu14hvm"
#define DOMAIN_NAME "xen16a"

// default return value of xen_get_domainid_from_name
// only used when fail to convert a string domain name to domain ID.
#define DOMAIN_ID  1 

#endif // DOMAIN_ID_H

