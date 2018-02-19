XEN_ROOT = $(CURDIR)/../..

include $(XEN_ROOT)/Config.mk

all: main.a 

main.a: main.o ./tiny-vmi/tiny_test.o ./tiny-vmi/tiny_xen.o ./tiny-vmi/tiny_cache.o ./tiny-vmi/mem_cache.o ./tiny-vmi/tiny_kv2p.o  ./tiny-vmi/tiny_vmi_init.o ./tiny-vmi/tiny_read.o ./tiny-vmi/tiny_GetVaOfVetorInIDT.o
	$(AR) cr $@ $^

clean:
	rm -f *.a *.o *~
	make clean -C tiny-vmi
	make clean -C include
