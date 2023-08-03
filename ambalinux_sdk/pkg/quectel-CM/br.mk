QL_CM_SRC=QmiWwanCM.c GobiNetCM.c main.c MPQMUX.c QMIThread.c util.c qmap_bridge_mode.c mbim-cm.c device.c
QL_CM_SRC+=atc.c atchannel.c at_tok.c
#QL_CM_SRC+=qrtr.c rmnetctl.c
ifeq (1,1)
QL_CM_DHCP=udhcpc.c
else
LIBMNL=libmnl/ifutils.c libmnl/attr.c libmnl/callback.c libmnl/nlmsg.c libmnl/socket.c
DHCP=libmnl/dhcp/dhcpclient.c libmnl/dhcp/dhcpmsg.c libmnl/dhcp/packet.c
QL_CM_DHCP=udhcpc_netlink.c
QL_CM_DHCP+=${LIBMNL}
endif

CFLAGS += -Wall -Wextra -O1 #-s -Werror
LDFLAGS += -lpthread -ldl -lrt
TARGET=quectel-CM quectel-qmi-proxy quectel-mbim-proxy quectel-atc-proxy

all: clean qmi-proxy mbim-proxy atc-proxy #qrtr-proxy
	$(CC) ${CFLAGS} ${QL_CM_SRC} ${QL_CM_DHCP} -o quectel-CM ${LDFLAGS}

debug: clean
	$(CC) ${CFLAGS} -g -DCM_DEBUG ${QL_CM_SRC} ${QL_CM_DHCP} -o quectel-CM -lpthread -ldl -lrt

qmi-proxy:
	$(CC) ${CFLAGS} quectel-qmi-proxy.c -o quectel-qmi-proxy ${LDFLAGS} 

mbim-proxy:
	$(CC) ${CFLAGS} quectel-mbim-proxy.c -o quectel-mbim-proxy ${LDFLAGS} 

qrtr-proxy:
	$(CC) ${CFLAGS} quectel-qrtr-proxy.c -o quectel-qrtr-proxy ${LDFLAGS} 

atc-proxy:
	$(CC) ${CFLAGS} quectel-atc-proxy.c atchannel.c at_tok.c util.c -o quectel-atc-proxy ${LDFLAGS} 

install: $(TARGET)
	mkdir -p $(DESTDIR)/usr/bin;
	for i in $^; do install -D -m 755  $$i $(DESTDIR)/usr/bin/$$i; done

clean:
	rm -rf *.o libmnl/*.o quectel-CM quectel-qmi-proxy quectel-mbim-proxy quectel-atc-proxy
