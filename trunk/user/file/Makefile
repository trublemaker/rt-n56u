VER=5.32
#http://ftp.astron.com/pub/file/file-5.32.tar.gz
FILE="http://ftp.astron.com/pub/file/file-$(VER).tar.gz"

SRC_NAME="file-$(VER)"


all: config_test build

config_test:
	( if [ -f ./config_done ]; then \
		echo "the same configuration"; \
	else \
		make configure && touch config_done; \
	fi )

configure:
	( cd $(SRC_NAME) ; \
	./configure \
		--prefix=/usr \
		--host=$(HOST_TARGET) ; \
	)

build:
	$(MAKE) -C $(SRC_NAME)

clean:
	if [ -f $(SRC_NAME)/Makefile ] ; then \
		$(MAKE) -C $(SRC_NAME) distclean ; \
	fi ; \
	rm -f config_done
	rm -f file

romfs:	
	mkdir -p $(INSTALLDIR)/usr/share/misc
	mkdir -p $(INSTALLDIR)/usr/bin
	mkdir -p $(INSTALLDIR)/usr/lib
	
	cp $(SRC_NAME)/src/.libs/file $(ROMFSDIR)/usr/bin/file
	cp $(SRC_NAME)/magic/magic.mgc $(ROMFSDIR)/usr/share/misc/magic.mgc
	cp $(SRC_NAME)/src/.libs/libmagic.so.1.0.0 $(ROMFSDIR)/usr/lib/libmagic.so.1.0.0
	cp -d $(SRC_NAME)/src/.libs/libmagic.so $(ROMFSDIR)/usr/lib/libmagic.so
	cp -d $(SRC_NAME)/src/.libs/libmagic.so.1 $(ROMFSDIR)/usr/lib/libmagic.so.1

	$(STRIP) $(ROMFSDIR)/usr/bin/file
	$(STRIP) $(ROMFSDIR)/usr/lib/libmagic.so.1.0.0
