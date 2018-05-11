#ifndef TARGET_LIBVMI_CONF_SYSMAP_H
#define TARGET_LIBVMI_CONF_SYSMAP_H


/**
* define system_map_string for Linux
* change this if use another system map
* all the System_map_* variables are declared here, and
* defined at file /tiny-vmi/config/libvmi_conf_file_sysmap_*.c
* the definition is generated by xxd command which coverts a file
* to ASCII string codes: xxd -i System_map_*

*/

#define linux_system_map_string_SRC_FILE "/tiny-vmi/config/libvmi_conf_file_sysmap_4_4_0_32.c"
#define linux_system_map_string System_map_4_4_0_31_generic_32
#define linux_system_map_string_len System_map_4_4_0_31_generic_32_len

extern unsigned char System_map_4_4_0_31_generic_32[];
extern unsigned int System_map_4_4_0_31_generic_32_len;

//#define linux_system_map_string_SRC_FILE "/tiny-vmi/config/libvmi_conf_file_sysmap_3_19_0_25_x64.c"
//#define linux_system_map_string System_map_3_19_0_25_generic
//#define linux_system_map_string_len System_map_3_19_0_25_generic_len
//extern unsigned char System_map_3_19_0_25_generic[];
//extern unsigned int System_map_3_19_0_25_generic_len;

#endif // TARGET_LIBVMI_CONF_SYSMAP_H