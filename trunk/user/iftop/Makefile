SRC_NAME = iftop-1.0pre4

THISDIR = $(shell pwd)

all: extract_test config_test
	$(MAKE) -j$(HOST_NCPU) -C $(SRC_NAME)

extract_test:
	( if [ ! -d $(SRC_NAME) ]; then \
			tar -xf $(SRC_NAME).tar.gz; \
	   else \
			cd $(SRC_NAME) ; \
			##git pull ;\
	fi )
	
config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )
#>$(ROOTDIR)/libs/libpcap/libpcap-1.9.0
# --host=mipsel-linux-uclibc  --with-libpcap=/mnt/sdb/home/mao/rt-n56u/trunk/stage   LDFLAGS=-L/mnt/sdb/home/mao/rt-n56u/trunk/stage/lib  \
#		CFLAGS=-I/mnt/sdb/home/mao/rt-n56u/trunk/stage/include \
#		LDFLAGS=-I/mnt/sdb/home/mao/rt-n56u/trunk/stage/lib \
		
configure:
	@echo $(ROOTDIR)/libs/libpcap/libpcap-1.9.0 
	( cd $(SRC_NAME) ; \
	./configure \
		--prefix=/usr \
		--with-libpcap=$(STAGEDIR) \
		--host=$(HOST_TARGET) \
		--build=$(HOST_BUILD) ; \
	)
	
clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi ; \
	rm -f config_done

romfs:
	$(ROMFSINST) -p +x $(THISDIR)/$(SRC_NAME)/iftop /usr/bin/iftop
