//#include <ntddk.h>
//#include <string.h>
#include "tiny_kv2p.h"
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
  P2C_U16 limit;    // 范围
  P2C_U32 base;    // 基地址（就是开始地址）
} P2C_IDTR, *PP2C_IDTR;
#pragma pack(pop)

addr_t GetVaOfVectorInIDT(vmi_instance_t vmi,int IndexInIDT);
void test_idt_vector(vmi_instance_t vmi,int IndexInIDT);
