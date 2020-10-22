/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
#include <sys/stat.h>
#include <stdint.h>
#include <fcntl.h>

//#include <include/bsd_queue.h>
//#include <rstats.h>

typedef struct {
	uint32_t xtime;
	uint64_t counter[2];
} data_t;

typedef struct {
	uint32_t id;
	data_t daily[62];
	int dailyp;
	data_t monthly[25];
	int monthlyp;
} history_t_old;

typedef struct {
	uint32_t id;
	data_t daily[90];
	int dailyp;
	data_t monthly[90];
	int monthlyp;
} history_t_new;

int main(int argc, char** argv){
    	int f,fd;
	int n,r;
        mode_t m;
        int i;
        
        history_t_new new;
        history_t_old old;
        
        memset(&new,0,sizeof(new));
        memset(&old,0,sizeof(old));
        
        printf("%d-%d  %s\n",sizeof(old),sizeof(new), argv[1]);
        
        //argv[1]
	if ((f = open("rstats-history", O_RDONLY)) < 0) return -1;
	n = read(f, &old, sizeof(history_t_old));
	close(f);
        
        //memcpy(&new,&old,sizeof(old));
        
        //new.id = old.id;
        new.id = old.id ;
        new.dailyp = 62;//old.dailyp ;
        new.monthlyp = 25;//old.monthlyp;
        
        printf("%08x-%08x-%08x  \n",old.id,old.dailyp, old.monthlyp);
           
        for( i =0 ;i < 62; i ++){
            new.daily[i]=old.daily[i];
        }        
        for( i =0 ;i < 25; i ++){
            new.monthly[i]=old.monthly[i];
        }
           
	m = umask(0);
	if ((fd = open("new", O_WRONLY|O_CREAT|O_TRUNC, 0666)) >= 0) {
            r = write(fd, &new, sizeof(history_t_new));
            close(fd);
	}
	umask(m);
        
	return r;    
}
