#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include<fcntl.h>

#if defined (USE_SINGLE_MAC)
	#define dev "eth2\n"
#else
	#define dev "eth3\n"
#endif

#define TIMESEC 2

/*
#define STR1(R) #R	 
#define STR2(R) STR1(R)

#pragma message "USE_SINGLE_MAC: " STR2( USE_SINGLE_MAC )
#pragma message "dev: " STR2( dev )

#define main xxxx
*/

/*

mtk_esw  11
WAN ports link state: 1

/proc/interrupts
11:          1          0 1202948276          0  MIPS GIC  eth2\0A

/sbin/mtk_esw  41 1 0
/sbin/mtk_esw  41 1 1

*/

#define MTK_ESW_DEVPATH                            "/dev/mtk_esw"
#define MTK_ESW_IOCTL_CMD_LENGTH_BITS              (8)

#define MTK_ESW_IOCTL_STATUS_LINK_PORTS_WAN        11
#define MTK_ESW_IOCTL_STATUS_LINK_PORTS_LAN        12


#ifdef debugprintf
#define debugpri(mesg, args...) fprintf(stderr, "[NetRate print:%s:%d:] " mesg "\n", __FILE__, __LINE__, ##args)
#else
#define debugpri(mesg, args...)
#endif

//# mtk_esw  41 1 0
//# mtk_esw  41 1 1

static int
mtk_esw_ioctl(unsigned int cmd, unsigned int par, unsigned int *value)
{
	int fd, retVal = 0;

	fd = open(MTK_ESW_DEVPATH, O_RDONLY);
	if (fd < 0)
	{
		perror(MTK_ESW_DEVPATH);
		return errno;
	}

	cmd &= ((1u << MTK_ESW_IOCTL_CMD_LENGTH_BITS) - 1);
	cmd |= (par << MTK_ESW_IOCTL_CMD_LENGTH_BITS);

	if (ioctl(fd, cmd, value) < 0) {
		perror("ioctl");
		retVal = errno;
	}

	close(fd);

	return retVal;
}

static int show_status_link(unsigned int cmd, unsigned int port_id)
{
	int retVal;
	unsigned int link_value = 0;
	const char *portname = NULL;

	retVal = mtk_esw_ioctl(cmd, port_id, &link_value);
	if (retVal != 0)
		return retVal;

	switch (cmd)
	{
	//case MTK_ESW_IOCTL_STATUS_LINK_PORT:
		//portname = get_port_name(port_id);
		//break;
	case MTK_ESW_IOCTL_STATUS_LINK_PORTS_WAN:
		portname = "WAN ports";
		break;
	case MTK_ESW_IOCTL_STATUS_LINK_PORTS_LAN:
		portname = "LAN ports";
		break;
	}

	if (!portname)
		return -1;

	//printf("%s link state: %d\n", portname, link_value);

	return link_value;
}


long long CalIrqSum(char* devname,FILE *fd)
{
	char buf[8192];
	char tempstr[16][16] = {0};

	long long interrupts[4] = {0};
	//char *dev = "eth0-TxRx-0\n";
	int i=0;

	fseek(fd, 0, SEEK_SET);
	int nBytes = fread(buf, 1, sizeof(buf) - 1, fd);
	if (-1 == nBytes)
	{
		debugpri("fread error");
		fclose(fd);
		return -1;
	}
	buf[nBytes] = '\0';

	char *end = strstr(buf, devname);
	char *start = end;
	int len = end - buf;

	end[4] = 0;

	//printf("len :   %d\n", len);

	while (start-- > buf)
	{
		if (*start == '\n')
		{
			//printf("lenstr :   %s\n", start);
			break;
		}
	}

	sscanf(start, "%*[^:]:%s%s%s%s%*s",
		   tempstr[0], tempstr[1], tempstr[2], tempstr[3]);
	//printf("tempstr :   %s %s %s %s\n", tempstr[0], tempstr[1], tempstr[2], tempstr[3]);

	long long sum = 0;
	long long sum_pre = 0;
	for ( i = 0; i < 4; i++)
	{
		interrupts[i] = atoll(tempstr[i]);
		//printf("%3d %s  %lld \n",i,tempstr[i],interrupts[i] );
		sum += interrupts[i];
	}

	//printf("%lld\n", sum);
	return sum;
}

int main(int argc, char *argv[])
{
	FILE *fstream = NULL;
	char buff[8192];
	int linkstate=0;
	
	unsigned long long count=0;

	memset(buff, 0, sizeof(buff));
	char tempstr[16][16] = {0};

	long long interrupts[4] = {0};
	unsigned int   arg = 1;
    struct timespec tv_now, tv_pre;
	double deltatime=0,recvrate=0;
	//if(1) return;

	FILE *fd = fopen("/proc/interrupts", "r");
	if (NULL == fd)
	{
		perror("open error!");
		//debugpri("/proc/interrupts not exists!\n");
		return -1;
	}

	long long sum=0;
	long long sum_pre=1;
	
	printf("watch wan\nCopyright (C) footlog.mao \n\n");

	if (daemon(0, 1) < 0) /*1 stdin/out/err no change,0 to null*/
		{
		perror("daemon");
		exit(errno);
	}

	while(1){

		linkstate=show_status_link(MTK_ESW_IOCTL_STATUS_LINK_PORTS_WAN,11);

		if(linkstate){
			clock_gettime(CLOCK_MONOTONIC ,&tv_now );
			sum = CalIrqSum(dev,fd);
			if(sum_pre==sum){
				printf("restart wan port.\n");//exit(0);
				//#mtk_esw  41 1 0   #mtk_esw  41 1 1 
				mtk_esw_ioctl(41,0,&arg);
				usleep(200*1000);
				mtk_esw_ioctl(41,1,&arg);
				sleep(1);
			}

			deltatime =tv_now.tv_sec + tv_now.tv_nsec * 0.000000001 - tv_pre.tv_sec - tv_pre.tv_nsec * 0.000000001;
			recvrate = (sum - sum_pre) / (deltatime); 
			//printf("eth2 irq rate: %g  %g  %d %d\n",recvrate,deltatime,tv_now.tv_sec,tv_pre.tv_sec);
			printf("eth2 %lld %lld irq rate: %7.2f  time:%8gs  count:%6llu\n",sum_pre,sum,recvrate,deltatime,count);
			sum_pre=sum;
			clock_gettime(CLOCK_MONOTONIC ,&tv_pre );

			//usleep(100*1000);
			sleep(TIMESEC);
		}
		else{
			sleep(TIMESEC);
		}
		
		count++;
	}

	return 0;
}
