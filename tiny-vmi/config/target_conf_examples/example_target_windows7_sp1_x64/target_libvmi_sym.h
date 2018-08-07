#ifndef TARGET_LIBVMI_SYM_H
#define TARGET_LIBVMI_SYM_H

/**
* If rekall file is used:
*
* - following https://github.com/google/rekall/tree/master/tools/linux to generate rekall files for linux
* - convert .json rekall file to C string using xxd -i
* 
*/

#ifdef SYM_FILE_FROM_STRING
#undef SYM_FILE_FROM_STRING
#endif

#ifndef REKALL_FILE_FROM_STRING
#define REKALL_FILE_FROM_STRING
#endif  //REKALL_FILE_FROM_STRING

#define guest_rekall_string windows7_sp1_rekall_json
#define guest_rekall_string_len windows7_sp1_rekall_json_len

extern unsigned char windows7_sp1_rekall_json[];
extern unsigned int windows7_sp1_rekall_json_len;

#define guest_rekall_string_SRC_FILE "/tiny-vmi/config/target_conf/target_libvmi_sym.c"


#endif // TARGET_LIBVMI_SYM_H

