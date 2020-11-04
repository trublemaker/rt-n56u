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
#include <time.h>

int main(){
    int r=0;
    struct timespec time1 = {0, 0};
 
    while(1){
        //CLOCK_MONOTONIC,CLOCK_BOOTTIME;
        clock_gettime(CLOCK_REALTIME, &time1);
        printf("%d\n", (int)time1.tv_sec);
        time1.tv_sec += 60*60;
    
        usleep (1000*1000);
    
        r = clock_settime(CLOCK_REALTIME, &time1); 
    
        if( r!=0){
            printf("clock_settime %d\t",r);
            continue;
        }
    
        //clock_gettime(CLOCK_REALTIME, &time1);
        //printf("%d\n",(int)time1.tv_sec);
    
    }

    return 0;
}