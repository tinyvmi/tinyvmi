XEN_ROOT = $(CURDIR)/../..

include $(XEN_ROOT)/Config.mk

SRC_CODE_H += ./include/tiny_private.h
SRC_CODE_H += ./include/debug.h
SRC_CODE_H += ./include/domain_id.h
SRC_CODE_H += ./include/tiny_cache.h
SRC_CODE_H += ./include/tiny_config.h
SRC_CODE_H += ./include/tiny_glib.h
SRC_CODE_H += ./include/tiny_libvmi.h
SRC_CODE_H += ./include/x86.h

SRC_CODE_C += main.c

SRC_CODE_C += ./tests/tiny_test.c
SRC_CODE_H += ./tests/tiny_test.h
SRC_CODE_H += ./tests/unit_tests/unit_tests.h
SRC_CODE_C += ./tests/unit_tests/unit_test_vmi_init.c

SRC_CODE_C += ./tests/examples/examples.c
SRC_CODE_H += ./tests/examples/examples.h
SRC_CODE_C += ./tests/examples/module-list.c
SRC_CODE_C += ./tests/examples/map-addr.c
SRC_CODE_C += ./tests/examples/event-example.c
SRC_CODE_C += ./tests/examples/interrupt-event-example.c
SRC_CODE_C += ./tests/examples/map-symbol.c
SRC_CODE_C += ./tests/examples/dump-memory.c
SRC_CODE_C += ./tests/examples/msr-event-example.c
SRC_CODE_C += ./tests/examples/process-list.c
# SRC_CODE_C += ./tests/examples/shm-snapshot-process-list.c # todo: not tested yet.
SRC_CODE_C += ./tests/examples/singlestep-event-example.c
SRC_CODE_C += ./tests/examples/step-event-example.c
SRC_CODE_C += ./tests/examples/va-pages.c
SRC_CODE_C += ./tests/examples/xen-emulate-response.c

 
SRC_CODE_C += ./tiny-vmi/cache.c
SRC_CODE_H += ./tiny-vmi/cache.h

# todo: might remove this? 
# The old tiny_list is replaced by queue from glib.
SRC_CODE_C += ./tiny-vmi/driver/tiny_memory_cache.c
SRC_CODE_C += ./tiny-vmi/driver/tiny_memory_cache_list.c

SRC_CODE_C += ./tiny-vmi/accessors.c
SRC_CODE_C += ./tiny-vmi/read.c
SRC_CODE_C += ./tiny-vmi/pretty_print.c
SRC_CODE_C += ./tiny-vmi/strmatch.c

SRC_CODE_C += ./tiny-vmi/events.c 
SRC_CODE_H += ./tiny-vmi/events.h

SRC_CODE_C += ./tiny-vmi/rekall.c 
SRC_CODE_H += ./tiny-vmi/rekall.h

SRC_CODE_C += ./tiny-vmi/driver/driver_interface.c
SRC_CODE_C += ./tiny-vmi/driver/xen/libxc_wrapper.c
SRC_CODE_C += ./tiny-vmi/driver/xen/libxs_wrapper.c
SRC_CODE_C +=  ./tiny-vmi/driver/xen/xen_events_legacy.c
SRC_CODE_C += ./tiny-vmi/driver/xen/xen_events_46.c 
SRC_CODE_C += ./tiny-vmi/driver/xen/xen_events_48.c
SRC_CODE_C += ./tiny-vmi/driver/xen/xen_events.c 
SRC_CODE_C += ./tiny-vmi/driver/xen/altp2m.c
SRC_CODE_C += ./tiny-vmi/driver/xen/xen.c

SRC_CODE_C = ./tiny-vmi/arch/arch_interface.c
SRC_CODE_C += ./tiny-vmi/arch/amd64.c
SRC_CODE_C += ./tiny-vmi/arch/arm_aarch32.c
SRC_CODE_C += ./tiny-vmi/arch/arm_aarch64.c
SRC_CODE_C += ./tiny-vmi/arch/intel.c

SRC_CODE_C += ./tiny-vmi/os/os_interface.c
SRC_CODE_C += ./tiny-vmi/os/linux/core.c
SRC_CODE_C += ./tiny-vmi/os/linux/memory.c
SRC_CODE_C += ./tiny-vmi/os/linux/symbols.c

SRC_CODE_C += ./tiny-vmi/os/windows/core.c
SRC_CODE_C += ./tiny-vmi/os/windows/kdbg.c
SRC_CODE_C += ./tiny-vmi/os/windows/memory.c
SRC_CODE_C += ./tiny-vmi/os/windows/peparse.c
SRC_CODE_C += ./tiny-vmi/os/windows/process.c
SRC_CODE_C += ./tiny-vmi/os/windows/symbols.c
SRC_CODE_C += ./tiny-vmi/os/windows/unicode.c

SRC_CODE_C += ./tiny-vmi/config/lexicon.c
SRC_CODE_C += ./tiny-vmi/config/grammar.c
SRC_CODE_C += ./tiny-vmi/config/libvmi_conf_file.c
SRC_CODE_C += ./tiny-vmi/config/target_conf/target_libvmi_conf_file.c
SRC_CODE_C += ./tiny-vmi/config/target_conf/target_libvmi_sym.c

SRC_CODE_C += ./tiny-vmi/driver/file/file.c
SRC_CODE_C += ./tiny-vmi/memory.c
SRC_CODE_C += ./tiny-vmi/tiny_core.c
SRC_CODE_C += ./tiny-vmi/convenience.c

SRC_CODE_C += ./tiny-vmi/tiny_glib/types.c
SRC_CODE_C += ./tiny-vmi/tiny_glib/slice.c
SRC_CODE_C += ./tiny-vmi/tiny_glib/hash.c
SRC_CODE_C += ./tiny-vmi/tiny_glib/list.c
SRC_CODE_C += ./tiny-vmi/tiny_glib/slist.c
SRC_CODE_C += ./tiny-vmi/tiny_glib/hashtable.c
SRC_CODE_C += ./tiny-vmi/tiny_glib/queue.c

# get objects *.o from *.c list
OBJS := $(patsubst %.c,%.o,$(SRC_CODE_C))

# set search path of header files
CFLAGS += -I../libxc-$(XEN_TARGET_ARCH)/include 
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/call/include
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/devicemodel/include
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/evtchn/include
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/foreignmemory/include
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/gnttab/include
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/toollog/include
CFLAGS += -I$(CURDIR)/include
CFLAGS += -I$(CURDIR)/tiny-vmi

CFLAGS += -DXC_WANT_COMPAT_EVTCHN_API -DXC_WANT_COMPAT_MAP_FOREIGN_API -DXC_WANT_COMPAT_DEVICEMODEL_API

all: main.a 

$(OBJS): $(SRC_CODE_H) $(SRC_CODE_C)

main.a: $(OBJS)
	$(AR) cr $@ $^


OB_CLEAN_DIR += tests
OB_CLEAN_DIR += tests/examples
OB_CLEAN_DIR += tests/unit_tests

OB_CLEAN_DIR += tiny-vmi
OB_CLEAN_DIR += tiny-vmi/arch
OB_CLEAN_DIR += tiny-vmi/config
OB_CLEAN_DIR += tiny-vmi/config/target_conf
OB_CLEAN_DIR += tiny-vmi/driver
OB_CLEAN_DIR += tiny-vmi/driver/file
OB_CLEAN_DIR += tiny-vmi/driver/xen
OB_CLEAN_DIR += tiny-vmi/os
OB_CLEAN_DIR += tiny-vmi/os/linux
OB_CLEAN_DIR += tiny-vmi/os/windows
OB_CLEAN_DIR += tiny-vmi/tiny_glib

# remove all object files, temporary files
clean:
	rm -f *.o *~
	(for sub_dir in $(OB_CLEAN_DIR); do \
	  	rm -f $$sub_dir/*.o ; \
		rm -f $$sub_dir/*~ ; \
	done; )
