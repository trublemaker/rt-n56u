EXEC = mcast speed rsconv

CC ?= mipsel-linux-uclibc-gcc 

CFLAGS += $(UOPT) -O3 #-s
#CFLAGS += -g
LDFLAGS = -s -lz -L/home/mao/smb/k2p/rt-n56u.hanwckf.new3/trunk/libs/libz/zlib-1.2.11/

#CFLAGS += $(if $(CONFIG_RAETH_GMAC2),,-DUSE_SINGLE_MAC)

all: $(EXEC)

	@# @echo "-$(CFLAGS)-$(LDFLAGS)-"
	
	#mipsel-linux-uclibc-strip  mcast
	#scp mcast jnkh.vicp.net:/tmp
	
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $^

mcast: mcast.o ifaddr.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

speed: speed.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	
rsconv: rsconv.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)
	
romfs:
	$(ROMFSINST) mcast /usr/bin/mcast
	$(ROMFSINST) speed /usr/bin/speed
clean:
	rm -f $(EXEC) *.o


