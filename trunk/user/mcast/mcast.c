/*
 *broadcast_client.c - �ಥ�Ŀͻ���
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/select.h>

#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/time.h>

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <assert.h>
#include <fcntl.h>
#include <syslog.h>
#include <time.h>
#include <sys/uio.h>

#include <sys/socket.h> 
#include <sys/epoll.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
 //#include <iostream>
#include <signal.h>

#include "ifaddr.h"

//#include <zlib.h>

//CCTV8HD 239.93.1.14.2225
//239.93.0.1:8000
//8000, "239.93.0.1"
//CCTV1HD 239.93.0.184.5140 239.93.0.184.5140

//CCTV-6-HD 239.93.1.14.2225

//CCTV-12���� 239.93.1.30:8124

#define MCAST_PORT    8224
#define MCAST_ADDR    "239.93.1.251"

//#define LOCAL_ADDR    "eth2.9"
char LOCAL_ADDR[64] = "eth2.9";
//viptv_ifname=eth2.9

/*һ���ֲ����Ӷಥ��ַ��·����������ת��*/

#define MCAST_INTERVAL 1 /*���ͼ��ʱ��*/

#define BUFF_SIZE 1500 /*���ջ�������С*/

/* set/clear file/socket's mode as non-blocking
 */
int
set_nblock(int fd, int set) {
	int flags = 0;
	printf("%s: fcntl() getting flags on fd=[%d].\n",	__func__, fd);

	flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) {
		printf(   "%s: fcntl() getting flags on fd=[%d]",
		            __func__, fd );
		return -1;
	}

	if (set)
		flags |= O_NONBLOCK;
	else
		flags &= ~O_NONBLOCK;

	if (fcntl(fd, F_SETFL, flags) < 0) {
		printf(   "%s: fcntl() %s non-blocking mode "
		        "on fd=[%d]", __func__, (set ? "setting" : "clearing"),
		        fd );
		return -1;
	}
	return 0;
}

int main_normal(int argc, char*argv[]) {

	int s; /*�׽����ļ�������*/

	struct sockaddr_in local_addr; /*���ص�ַ*/
	struct sockaddr_in rcv_addr; /*���ص�ַ*/

	int err = -1;

	char maddr[64];
	int mport = 0;
	char mladdr[64];

	printf("usage: mcast x.x.x.x port a.a.a.a \nor\n");
	printf("usage: mcast x.x.x.x port ifname.\n\n");

	if (argc == 4) {
		strcpy(maddr, argv[1]);
		mport = atoi(argv[2]);
		strcpy(mladdr, argv[3]);
	}
	else {
		strcpy(maddr, MCAST_ADDR);
		mport = MCAST_PORT;
		strcpy(mladdr, LOCAL_ADDR);
	}

	if (0 != get_ipv4_address(mladdr, mladdr, sizeof(mladdr))) {
		perror("Get multicast address error ." );
		return -1;
	}
	else {
		printf("addr:%s.\n", mladdr);
	}

	// MCAST_PORT    MCAST_ADDR   
	printf("MCast:%s:%d\n", maddr, mport);

	s = socket(AF_INET, SOCK_DGRAM, 0); /*�����׽���*/

	if (s == -1) {
		perror("socket()");
		return -1;
	}

	/*��ʼ����ַ*/

	memset(&local_addr, 0, sizeof(local_addr));

	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = inet_addr(maddr); //htonl(INADDR_ANY);
	local_addr.sin_port = htons(mport);

	/*��socket*/

	err = bind(s, (struct sockaddr*) &local_addr, sizeof(local_addr));

	if (err < 0) {
		perror("bind()");
		return -2;
	}

	/*���ûػ����*/

	int loop = 1;

	err = setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));

	if (err < 0) {
		perror("setsockopt():IP_MULTICAST_LOOP");
		return -3;
	}

	struct ip_mreq mreq; /*����㲥��*/

	mreq.imr_multiaddr.s_addr = inet_addr(maddr); /*�㲥��ַ*/

	printf("%08X -- %08X\n", inet_addr(maddr), inet_addr(mladdr));

	mreq.imr_interface.s_addr = inet_addr(mladdr); //htonl(INADDR_ANY); 
	//mreq.imr_interface.s_addr    = htonl(INADDR_ANY); 

	/*����ӿ�ΪĬ��*/

	/*����������㲥��*/

	err = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

	if (err < 0) {
		perror("setsockopt():IP_ADD_MEMBERSHIP");
		return -4;
	}

	printf("setsockopt():IP_ADD_MEMBERSHIP OK. ---\n");

	int times = 0;

	int addr_len = 0;

	unsigned char buff[BUFF_SIZE]={0};

	int n = 0;

	set_nblock(s, 0);

	int ret = 0;

	/*ѭ�����չ㲥�����Ϣ��5000�κ��˳�*/

	addr_len = sizeof(rcv_addr);
	
	for (times = 0; 1; times++) //times<50000
	{
		//memset(buff, 0, BUFF_SIZE); /*��ս��ջ�����*/

		//memset(&rcv_addr, 0, sizeof(rcv_addr));

		/*��������*/
		//n = recvfrom(s, buff, BUFF_SIZE, 0, (struct sockaddr*) &rcv_addr,	&addr_len);
		n = recv(s, buff, BUFF_SIZE, 0);
		
		if (n == -1) {
			perror("recvfrom()");
		}
		else /*��ӡ��Ϣ*/ {
			if (n > 0)buff[n] = 0;
			//if (n > 6)buff[6] = 0;
			
			//int compress(Bytef *dest, uLongf *destLen, const Bytef *source, uLong sourceLen);
			//int compress2(Bytef *dest, uLongf *destLen, const Bytef *source, uLongsourceLen, int level); 
			char  dest[2000];
			//uLongf destLen=2000;
			int val=0;
			
			//val = compress2(dest, &destLen, buff, n,5);
			
			if(val == 1){ //
			 //printf("Recv %5d pack, %d bytes, compress to %d bytes. ......\n",
			//		times, n,
			//		(int)destLen
			//	);
			}
			else
			printf("%d *Recv %5d pack, %d bytes, [%08x]......\n",
				val , times, n,
				 ntohl(*(int*)&buff[0])
				);
		}

		sleep(0);
	}

	/*�˳��㲥��*/

	err = setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));

	close(s);

	return 0;

}

int main_select(int argc, char*argv[]) {

	int s; /*�׽����ļ�������*/

	struct sockaddr_in local_addr; /*���ص�ַ*/
	struct sockaddr_in rcv_addr; /*���ص�ַ*/

	int err = -1;

	char maddr[64];
	int mport = 0;
	char mladdr[64];

	printf("usage: mcast x.x.x.x port a.a.a.a \nor\n");
	printf("usage: mcast x.x.x.x port ifname.\n\n");

	if (argc == 4) {
		strcpy(maddr, argv[1]);
		mport = atoi(argv[2]);
		strcpy(mladdr, argv[3]);
	}
	else {
		strcpy(maddr, MCAST_ADDR);
		mport = MCAST_PORT;
		strcpy(mladdr, LOCAL_ADDR);
	}

	if (0 != get_ipv4_address(mladdr, mladdr, sizeof(mladdr))) {
		perror("get multicast address error.");
		return -1;
	}
	else {
		printf("addr:%s.\n", mladdr);
	}

	// MCAST_PORT    MCAST_ADDR   
	printf("MCast:%s:%d\n", maddr, mport);

	s = socket(AF_INET, SOCK_DGRAM, 0); /*�����׽���*/

	if (s == -1) {
		perror("socket()");
		return -1;
	}

	/*��ʼ����ַ*/

	memset(&local_addr, 0, sizeof(local_addr));

	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = inet_addr(maddr); //htonl(INADDR_ANY);
	local_addr.sin_port = htons(mport);

	/*��socket*/

	err = bind(s, (struct sockaddr*) &local_addr, sizeof(local_addr));

	if (err < 0) {
		perror("bind()");
		return -2;
	}

	/*���ûػ����*/

	int loop = 1;

	err = setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));

	if (err < 0) {
		perror("setsockopt():IP_MULTICAST_LOOP");
		return -3;
	}

	struct ip_mreq mreq; /*����㲥��*/

	mreq.imr_multiaddr.s_addr = inet_addr(maddr); /*�㲥��ַ*/

	printf("%08X -- %08X\n", inet_addr(maddr), inet_addr(mladdr));

	mreq.imr_interface.s_addr = inet_addr(mladdr); //htonl(INADDR_ANY); 
	//mreq.imr_interface.s_addr    = htonl(INADDR_ANY); 

	/*����ӿ�ΪĬ��*/

	/*����������㲥��*/

	err = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

	if (err < 0) {
		perror("setsockopt():IP_ADD_MEMBERSHIP");
		return -4;
	}

	printf("setsockopt():IP_ADD_MEMBERSHIP OK. ---\n");

	int times = 0;

	int addr_len = 0;

	char buff[BUFF_SIZE];

	int n = 0;

	set_nblock(s, 0);

	fd_set read_fds;  //���ļ�������
	fd_set exception_fds; //�쳣�ļ�������
	FD_ZERO(&read_fds);
	FD_ZERO(&exception_fds);
	int ret = 0;

	/*ѭ�����չ㲥�����Ϣ��5000�κ��˳�*/

	FD_SET(s, &read_fds);
	FD_SET(s, &exception_fds);

	for (times = 0; 1; times++) //times<50000
	{
		addr_len = sizeof(rcv_addr);
		//memset(buff, 0, BUFF_SIZE); /*��ս��ջ�����*/

		//memset(&rcv_addr, 0, sizeof (rcv_addr));		

		ret = select(s + 1, &read_fds, NULL, &exception_fds, NULL);
		if (ret < 0)
		{
			printf("Fail to select!\n");
			return -1;
		}


		if (FD_ISSET(s, &read_fds))
		{
			ret = recv(s, buff, sizeof(buff) - 1, 0);
			if (ret <= 0)
			{
				break;
			}

			//printf("get %d bytes of normal data: %x \n", ret, (long)buff[0]);

		}
		else if (FD_ISSET(s, &exception_fds)) //�쳣�¼�
		{
			ret = recv(s, buff, sizeof(buff) - 1, MSG_OOB);
			if (ret <= 0)
			{
				break;
			}

			//printf("get %d bytes of exception data: %s \n", ret, buff);
		}

		/*��������*/
		//n = recvfrom(s, buff, BUFF_SIZE, 0, (struct sockaddr*) &rcv_addr,  &addr_len);
		//n = read( s, buff, sizeof(buff) );

		if (n == -1) {
			//perror("recvfrom()");
		}
		else /*��ӡ��Ϣ*/ {
			//if (n > 0)buff[n] = 0;
			//if (n > 10)buff[10] = 0;

			//printf("Recv %4d st, %d bytes from server: [%08X]\n",
			//    times, n,
			//    (long)buff[0] );
		}

		//sleep(MCAST_INTERVAL);
	}

	/*�˳��㲥��*/

	err = setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof
	(mreq));

	close(s);

	return 0;

}

int main_epoll(int argc, char*argv[]) {

	int s; /*�׽����ļ�������*/

	struct sockaddr_in local_addr; /*���ص�ַ*/
	struct sockaddr_in rcv_addr; /*���ص�ַ*/

	int err = -1;

	char maddr[64];
	int mport = 0;
	char mladdr[64];
	int ret = 0;

	int times = 0;

	int addr_len = 0;

	char buff[BUFF_SIZE];

	int n = 0;
	int j;

	int epfd;        //�����õ�epoll
	struct epoll_event ev; //�¼���ʱ����


	printf("usage: mcast x.x.x.x port a.a.a.a \nor\n");
	printf("usage: mcast x.x.x.x port ifname.\n\n");

	if (argc == 4) {
		strcpy(maddr, argv[1]);
		mport = atoi(argv[2]);
		strcpy(mladdr, argv[3]);
	}
	else {
		strcpy(maddr, MCAST_ADDR);
		mport = MCAST_PORT;
		strcpy(mladdr, LOCAL_ADDR);
	}

	if (0 != get_ipv4_address(mladdr, mladdr, sizeof(mladdr))) {
		perror("get multicast address error.");
		return -1;
	}
	else {
		printf("addr:%s.\n", mladdr);
	}

	// MCAST_PORT    MCAST_ADDR   
	printf("MCast:%s:%d\n", maddr, mport);

	s = socket(AF_INET, SOCK_DGRAM, 0); /*�����׽���*/

	if (s == -1) {
		perror("socket()");
		return -1;
	}

	/*��ʼ����ַ*/

	memset(&local_addr, 0, sizeof(local_addr));

	local_addr.sin_family = AF_INET;
	local_addr.sin_addr.s_addr = inet_addr(maddr); //htonl(INADDR_ANY);
	local_addr.sin_port = htons(mport);

	/*��socket*/

	err = bind(s, (struct sockaddr*) &local_addr, sizeof(local_addr));

	if (err < 0) {
		perror("bind()");
		return -2;
	}

	/*���ûػ����*/

	int loop = 1;

	err = setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));

	if (err < 0) {
		perror("setsockopt():IP_MULTICAST_LOOP");
		return -3;
	}

	struct ip_mreq mreq; /*����㲥��*/

	mreq.imr_multiaddr.s_addr = inet_addr(maddr); /*�㲥��ַ*/

	printf("%08X -- %08X\n", inet_addr(maddr), inet_addr(mladdr));

	mreq.imr_interface.s_addr = inet_addr(mladdr); //htonl(INADDR_ANY); 
	//mreq.imr_interface.s_addr    = htonl(INADDR_ANY); 

	/*����ӿ�ΪĬ��*/

	/*����������㲥��*/

	err = setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));

	if (err < 0) {
		perror("setsockopt():IP_ADD_MEMBERSHIP");
		return -4;
	}

	printf("setsockopt():IP_ADD_MEMBERSHIP OK.\n");

	set_nblock(s, 0);

	//fd_set read_fds;  //���ļ�������
	//fd_set exception_fds; //�쳣�ļ�������
	//FD_ZERO(&read_fds);
	//FD_ZERO(&exception_fds);

	//sleep(1000);

	epfd = epoll_create(20);
	ev.events = EPOLLIN | EPOLLET;

	printf("epfd: %d \n", epfd);

	/*ѭ�����չ㲥�����Ϣ��5000�κ��˳�*/

	//FD_SET(s,&read_fds);
	//FD_SET(s,&exception_fds);

	ret = epoll_ctl(epfd, EPOLL_CTL_ADD, s, &ev);
	printf("2.ret: %d \n", ret);

	for (times = 0; 1; times++) //times<50000
	{
		addr_len = sizeof(rcv_addr);
		//memset(buff, 0, BUFF_SIZE); /*��ս��ջ�����*/

		//memset(&rcv_addr, 0, sizeof (rcv_addr));		

		//ret = select(s+1,&read_fds,NULL,&exception_fds,NULL);
		int retval = epoll_wait(epfd, &ev, 20, -1); //EPOLL_MAX_EVENTS
		//printf("%4d    ",retval);
		if (retval > 0) {
			for (j = 0; j < retval; j++) {
				// ������¼�
				if (ev.events & EPOLLIN) {
					//handle_ready_read_connection(conn);
					n = read(s, buff, sizeof(buff));

					if (n == -1) {
						perror("recvfrom()");
					}
					else /*��ӡ��Ϣ*/ {
						if (n > 0)buff[n] = 0;
						if (n > 10)buff[10] = 0;

						//printf("Recv %4d st, %d bytes from server: [%08X]\n",
						//	times, n,
						//	(long)buff[0]);
					}
					continue;
				}

			}
		}
	}
	/*�˳��㲥��*/

	err = setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof
	(mreq));

	close(s);

	return 0;

}

int main(int argc, char*argv[]) {
	//viptv_ifname=eth2.9
	FILE *wget;
	char ok_code[] = "ok";
	char wget_content[1024];
	wget=popen("nvram  get viptv_ifname","r");
	fgets(wget_content, sizeof(wget_content),wget);
	
	if(strlen(wget_content) <=0 ){
		printf("no iptv interface, return.\n");
		return -1;
	}
	//if(strcmp(wget_content, wait_code) == 0)
	{
		printf("iptv interface: %s\n",wget_content);
	}

	//main_select(argc, argv);
	main_normal(argc, argv);
	//main_epoll(argc, argv);
	return 0;
}
