XEN_ROOT = $(CURDIR)/../..

include $(XEN_ROOT)/Config.mk

CFLAGS += -DXC_WANT_COMPAT_EVTCHN_API -DXC_WANT_COMPAT_MAP_FOREIGN_API -DXC_WANT_COMPAT_DEVICEMODEL_API

SRC_CODE_H += ./include/tiny_private.h
SRC_CODE_H += ./include/debug.h
SRC_CODE_H += ./include/domain_id.h
SRC_CODE_H += ./include/tiny_cache.h
SRC_CODE_H += ./include/tiny_config.h
SRC_CODE_H += ./include/tiny_glib.h
SRC_CODE_H += ./include/tiny_libvmi.h
SRC_CODE_H += ./include/x86.h



# OBJS=main.o 
SRC_CODE_C += main.c

OB_tests += ./tests/tiny_test.o
SRC_CODE_C += ./tests/tiny_test.c
SRC_CODE_H += ./tests/tiny_test.h

SRC_CODE_H += ./tests/unit_tests/unit_tests.h

# OB_tests += ./tests/unit_tests/unit_test_vmi_init.o
SRC_CODE_C += ./tests/unit_tests/unit_test_vmi_init.c

# OB_tests += ./tests/examples/examples.o
SRC_CODE_C += ./tests/examples/examples.c
SRC_CODE_H += ./tests/examples/examples.h

# OB_test_module_list += ./tests/examples/module-list.o
SRC_CODE_C += ./tests/examples/module-list.c

# OB_test_map_addr += ./tests/examples/map-addr.o
SRC_CODE_C += ./tests/examples/map-addr.c

# OB_test_event += ./tests/examples/event-example.o
SRC_CODE_C += ./tests/examples/event-example.c

# OB_test_event += ./tests/examples/interrupt-event-example.o
SRC_CODE_C += ./tests/examples/interrupt-event-example.c

# OB_test_map_addr += ./tests/examples/map-symbol.o
SRC_CODE_C += ./tests/examples/map-symbol.c

# OB_tests += $(OB_test_map_addr) $(OB_test_module_list) $(OB_test_event)

# OB_tests += ./tests/examples/dump-memory.o
# OB_tests += ./tests/examples/msr-event-example.o
# OB_tests += ./tests/examples/process-list.o
# # OB_tests += ./tests/examples/shm-snapshot-process-list.o
# OB_tests += ./tests/examples/singlestep-event-example.o
# OB_tests += ./tests/examples/step-event-example.o
# OB_tests += ./tests/examples/va-pages.o
# OB_tests += ./tests/examples/xen-emulate-response.o
# 
# OBJS += $(OB_tests)

SRC_CODE_C += ./tests/examples/dump-memory.c
SRC_CODE_C += ./tests/examples/msr-event-example.c
SRC_CODE_C += ./tests/examples/process-list.c
# SRC_CODE_C += ./tests/examples/shm-snapshot-process-list.c
SRC_CODE_C += ./tests/examples/singlestep-event-example.c
SRC_CODE_C += ./tests/examples/step-event-example.c
SRC_CODE_C += ./tests/examples/va-pages.c
SRC_CODE_C += ./tests/examples/xen-emulate-response.c

 
# OBJS += ./tiny-vmi/cache.o
SRC_CODE_C += ./tiny-vmi/cache.c
SRC_CODE_H += ./tiny-vmi/cache.h

# OBJS += ./tiny-vmi/driver/tiny_memory_cache.o
# OBJS += ./tiny-vmi/driver/tiny_memory_cache_list.o
SRC_CODE_C += ./tiny-vmi/driver/tiny_memory_cache.c
SRC_CODE_C += ./tiny-vmi/driver/tiny_memory_cache_list.c

# OBJS += ./tiny-vmi/accessors.o  
SRC_CODE_C += ./tiny-vmi/accessors.c

# OBJS += ./tiny-vmi/read.o 
SRC_CODE_C += ./tiny-vmi/read.c

# OBJS += ./tiny-vmi/tiny_GetVaOfVetorInIDT.o

# OBJS += ./tiny-vmi/pretty_print.o
SRC_CODE_C += ./tiny-vmi/pretty_print.c

# OBJS += ./tiny-vmi/strmatch.o
SRC_CODE_C += ./tiny-vmi/strmatch.c

# OB_events += ./tiny-vmi/events.o
SRC_CODE_C += ./tiny-vmi/events.c 
SRC_CODE_H += ./tiny-vmi/events.h

# OB_REKALL += ./tiny-vmi/rekall.o
SRC_CODE_C += ./tiny-vmi/rekall.c 
SRC_CODE_H += ./tiny-vmi/rekall.h

# OB_drivers = ./tiny-vmi/driver/driver_interface.o 
SRC_CODE_C += ./tiny-vmi/driver/driver_interface.c

# OBJS += $(OB_events) $(OB_REKALL) $(OB_drivers) 


# OB_xc = ./tiny-vmi/driver/xen/libxc_wrapper.o
# OB_xs = ./tiny-vmi/driver/xen/libxs_wrapper.o

SRC_CODE_C += ./tiny-vmi/driver/xen/libxc_wrapper.c
SRC_CODE_C += ./tiny-vmi/driver/xen/libxs_wrapper.c

# OB_xen_events = ./tiny-vmi/driver/xen/xen_events_legacy.o
# OB_xen_events += ./tiny-vmi/driver/xen/xen_events_46.o
# OB_xen_events += ./tiny-vmi/driver/xen/xen_events_48.o
# OB_xen_events += ./tiny-vmi/driver/xen/xen_events.o

SRC_CODE_C +=  ./tiny-vmi/driver/xen/xen_events_legacy.c
SRC_CODE_C += ./tiny-vmi/driver/xen/xen_events_46.c 
SRC_CODE_C += ./tiny-vmi/driver/xen/xen_events_48.c
SRC_CODE_C += ./tiny-vmi/driver/xen/xen_events.c 

# OB_xen_altp2m = ./tiny-vmi/driver/xen/altp2m.o
SRC_CODE_C += ./tiny-vmi/driver/xen/altp2m.c

# OB_xen = ./tiny-vmi/driver/xen/xen.o
SRC_CODE_C += ./tiny-vmi/driver/xen/xen.c

OB_arch = ./tiny-vmi/arch/arch_interface.o
OB_arch += ./tiny-vmi/arch/amd64.o
OB_arch += ./tiny-vmi/arch/arm_aarch32.o
OB_arch += ./tiny-vmi/arch/arm_aarch64.o
OB_arch += ./tiny-vmi/arch/intel.o

OB_arch += ./tiny-vmi/os/os_interface.o
OB_arch += ./tiny-vmi/os/linux/core.o
OB_arch += ./tiny-vmi/os/linux/memory.o
OB_arch += ./tiny-vmi/os/linux/symbols.o

OB_arch += ./tiny-vmi/os/windows/core.o
OB_arch += ./tiny-vmi/os/windows/kdbg.o
OB_arch += ./tiny-vmi/os/windows/memory.o
OB_arch += ./tiny-vmi/os/windows/peparse.o
OB_arch += ./tiny-vmi/os/windows/process.o
OB_arch += ./tiny-vmi/os/windows/symbols.o
OB_arch += ./tiny-vmi/os/windows/unicode.o

SRC_ARCH := $(patsubst %.o,%.c,$(OB_arch))
SRC_CODE_C += $(SRC_ARCH)

OB_config += ./tiny-vmi/config/lexicon.o
OB_config += ./tiny-vmi/config/grammar.o
OB_config += ./tiny-vmi/config/libvmi_conf_file.o
OB_config += ./tiny-vmi/config/target_conf/target_libvmi_conf_file.o
OB_config += ./tiny-vmi/config/target_conf/target_libvmi_sym.o

SRC_CONFIG := $(patsubst %.o,%.c,$(OB_config))
SRC_CODE_C += $(SRC_CONFIG)

# OBJS += $(OB_xc) $(OB_xs) $(OB_xen_events) $(OB_xen_altp2m) $(OB_xen) $(OB_arch) $(OB_config)

# OBJS += ./tiny-vmi/driver/file/file.o
# OBJS += ./tiny-vmi/memory.o
# OBJS += ./tiny-vmi/tiny_core.o
# OBJS += ./tiny-vmi/convenience.o

SRC_CODE_C += ./tiny-vmi/driver/file/file.c
SRC_CODE_C += ./tiny-vmi/memory.c
SRC_CODE_C += ./tiny-vmi/tiny_core.c
SRC_CODE_C += ./tiny-vmi/convenience.c

OB_glib += ./tiny-vmi/tiny_glib/types.o
OB_glib += ./tiny-vmi/tiny_glib/slice.o
OB_glib += ./tiny-vmi/tiny_glib/hash.o
OB_glib += ./tiny-vmi/tiny_glib/list.o
OB_glib += ./tiny-vmi/tiny_glib/slist.o
OB_glib += ./tiny-vmi/tiny_glib/hashtable.o
OB_glib += ./tiny-vmi/tiny_glib/queue.o

SRC_GLIB := $(patsubst %.o,%.c,$(OB_glib))
SRC_CODE_C += $(SRC_GLIB)

# OBJS += $(OB_glib)

CFLAGS += -I../libxc-$(XEN_TARGET_ARCH)/include 
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/call/include
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/devicemodel/include
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/evtchn/include
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/foreignmemory/include
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/gnttab/include
CFLAGS += -I../libs-$(XEN_TARGET_ARCH)/toollog/include
CFLAGS += -I$(CURDIR)/include
CFLAGS += -I$(CURDIR)/tiny-vmi
# CFLAGS += -I$(CROSS_PREFIX)/$(GNU_TARGET_ARCH)-xen-elf/include 

# CFLAGS += -DENABLE_XEN=1 

# TARGET_CPPFLAGS += -isystem $(CURDIR)/libxc-$(XEN_TARGET_ARCH)/include #added 2018.03.05, lele
# TARGET_CPPFLAGS += -isystem $(CURDIR)/libs-$(XEN_TARGET_ARCH)/call/include #added 2018.03.05, lele
# TARGET_CPPFLAGS += -isystem $(CURDIR)/libs-$(XEN_TARGET_ARCH)/devicemodel/include #added 2018.03.05, lele
# TARGET_CPPFLAGS += -isystem $(CURDIR)/libs-$(XEN_TARGET_ARCH)/evtchn/include #added 2018.03.05, lele
# TARGET_CPPFLAGS += -isystem $(CURDIR)/libs-$(XEN_TARGET_ARCH)/foreignmemory/include #added 2018.03.05, lele
# TARGET_CPPFLAGS += -isystem $(CURDIR)/libs-$(XEN_TARGET_ARCH)/gnttab/include #added 2018.03.05, lele
# TARGET_CPPFLAGS += -isystem $(CURDIR)/libs-$(XEN_TARGET_ARCH)/toollog/include #added 2018.03.05, lele

# TARGET_CPPFLAGS += -I$(CURDIR)/tinyvmi/include #added 2014.10.27, lele
# TARGET_CPPFLAGS += -I$(CROSS_PREFIX)/$(GNU_TARGET_ARCH)-xen-elf/include  # added 2014.10.27

# LDFLAGS += -nostdlib -L$(CROSS_PREFIX)/$(GNU_TARGET_ARCH)-xen-elf/lib -L$(CURDIR)/xenstore -lxenstore

# Add libjson-c
# JSON_C_DIR=$(CROSS_ROOT)/$(GNU_TARGET_ARCH)-xen-elf
# CFLAGS += -I$(JSON_C_DIR)/include/json-c

LDFLAGS += -ljson-c
TARGET_LDFLAGS += -ljson-c


all: main.a 

# $(OB_config): $(OB_glib)

# $(OB_xen): $(OB_xc) $(OB_xs)

# $(OB_test_event): $(OB_events)

# tiny_read.o: $(OB_xen)

OBJS := $(patsubst %.c,%.o,$(SRC_CODE_C))

# $(OBJS): get_config_string $(SRC_CODE)
$(OBJS): $(SRC_CODE)

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


clean:
	rm -f *.o *~
	(for sub_dir in $(OB_CLEAN_DIR); do \
	  	rm -f $$sub_dir/*.o ; \
		rm -f $$sub_dir/*~ ; \
	done; )

