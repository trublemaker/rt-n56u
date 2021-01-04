SRC_NAME=3proxy-git

all:
	$(MAKE) -C $(SRC_NAME) -f Makefile.mips

clean:
	$(MAKE) -C $(SRC_NAME)  -f Makefile.mips clean

distclean:
	$(MAKE) -C $(SRC_NAME) -f Makefile.mips distclean
	
romfs:
	$(ROMFSINST) $(SRC_NAME)/src/3proxy /usr/bin/3proxy
	$(ROMFSINST) $(SRC_NAME)/src/tcppm /usr/bin/tcppm
	$(ROMFSINST) $(SRC_NAME)/src/udppm /usr/bin/udppm
	$(ROMFSINST) $(SRC_NAME)/src/socks /usr/bin/socks
	$(ROMFSINST) $(SRC_NAME)/cfg/3proxy.cfg.sample /etc/3proxy.cfg.sample
