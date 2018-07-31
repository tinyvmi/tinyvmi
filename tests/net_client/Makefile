#CFLAGS+= -isystem ./lwip/src/include 
#CFLAGS+= -isystem ./lwip/src/include/ipv4 
CFLAGS+= -nostdlib -isystem ../../../lwip-x86_64/src/include 
CFLAGS+= -isystem ../../../lwip-x86_64/src/include/ipv4 

CFLAGS+= -isystem ../../../../extras/mini-os/include/x86
CFLAGS+= -isystem ../../../../extras/mini-os/include/x86/x86_64
CFLAGS+= -isystem ../../../../extras/mini-os/include

#CFLAGS+= -isystem ./include

LDFLAGS=../../../mini-os-x86_64-tinyvmi/lwip.a 

# LWIPDIR=./lwip
# LWC	:= $(sort $(shell find $(LWIPDIR)/src -type f -name '*.c'))
# LWC	:= $(filter-out %6.c %ip6_addr.c %ethernetif.c, $(LWC))
# LWO	:= $(patsubst %.c,%.o,$(LWC))

all: client

# lwip.a: $(LWO)
# 	ar cqs ./lwip.a $(LWO)
# 	#ar cqs ./lwip.a ./lwip/src/api/api_lib.o ./lwip/src/api/api_msg.o ./lwip/src/api/err.o ./lwip/src/api/netbuf.o ./lwip/src/api/netdb.o ./lwip/src/api/netifapi.o ./lwip/src/api/sockets.o ./lwip/src/api/tcpip.o ./lwip/src/core/dhcp.o ./lwip/src/core/dns.o ./lwip/src/core/init.o ./lwip/src/core/ipv4/autoip.o ./lwip/src/core/ipv4/icmp.o ./lwip/src/core/ipv4/igmp.o ./lwip/src/core/ipv4/inet.o ./lwip/src/core/ipv4/inet_chksum.o ./lwip/src/core/ipv4/ip.o ./lwip/src/core/ipv4/ip_addr.o ./lwip/src/core/ipv4/ip_frag.o ./lwip/src/core/mem.o ./lwip/src/core/memp.o ./lwip/src/core/netif.o ./lwip/src/core/pbuf.o ./lwip/src/core/raw.o ./lwip/src/core/snmp/asn1_dec.o ./lwip/src/core/snmp/asn1_enc.o ./lwip/src/core/snmp/mib2.o ./lwip/src/core/snmp/mib_structs.o ./lwip/src/core/snmp/msg_in.o ./lwip/src/core/snmp/msg_out.o ./lwip/src/core/stats.o ./lwip/src/core/sys.o ./lwip/src/core/tcp.o ./lwip/src/core/tcp_in.o ./lwip/src/core/tcp_out.o ./lwip/src/core/udp.o ./lwip/src/netif/etharp.o ./lwip/src/netif/loopif.o ./lwip/src/netif/ppp/auth.o ./lwip/src/netif/ppp/chap.o ./lwip/src/netif/ppp/chpms.o ./lwip/src/netif/ppp/fsm.o ./lwip/src/netif/ppp/ipcp.o ./lwip/src/netif/ppp/lcp.o ./lwip/src/netif/ppp/magic.o ./lwip/src/netif/ppp/md5.o ./lwip/src/netif/ppp/pap.o ./lwip/src/netif/ppp/ppp.o ./lwip/src/netif/ppp/ppp_oe.o ./lwip/src/netif/ppp/randm.o ./lwip/src/netif/ppp/vj.o ./lwip/src/netif/slipif.o ./lwip_client.o
lwip_client.o: lwip_client.c
	gcc -c $(CFLAGS) lwip_client.c -o lwip_client.o

client: lwip_client.o 
	ld $(LDFLAGS) lwip_client.o -o client

# ld -r  /home/smeller/lab/libvmi/xen-4.10.0/stubdom/mini-os-x86_64-tinynet/lwip.a -L/home/smeller/lab/libvmi/xen-4.10.0/stubdom/mini-os-x86_64-tinynet/arch/x86 -lx86_64  -lc -o /home/smeller/lab/libvmi/xen-4.10.0/stubdom/mini-os-x86_64-tinynet/mini-os.o
