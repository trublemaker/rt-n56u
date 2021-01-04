#!/bin/sh


# File:   watch_wan.sh
# Author: footlog.mao
 
# Created on 12/01/2020

 
irqold=`cat /proc/interrupts | grep eth2 | awk '{print $4}'`

#   CPU0 CPU1 CPU2     CPU3   
#11: 21   0   16363663 0  MIPS GIC  eth2

echo "eth eth irqs " $irqold

while :
do
        linkstate=`mtk_esw  11 | awk '{print $5}'`
        #echo $linkstate

        if [ "$linkstate" -eq "1" ] 
        then
                sleep 1
                irqs=`cat /proc/interrupts | grep eth2 | awk '{print $4}'`
                #echo "$irqold  "   "    $irqs"
                if [ "$irqold" -eq "$irqs" ] 
                then
                        logger  "restart WAN" 
                        /sbin/mtk_esw  41 1 0
                        /sbin/mtk_esw  41 1 1
                        echo "restart WAN"
                        sleep 4
                fi
                irqold=$irqs
        fi
		
		sleep 1
done
