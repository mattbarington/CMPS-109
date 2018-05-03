#!/bin/bash

#######################################################################
#
# Copyright (C) 2018 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

if [ "$#" -ne 1 ]; then
    echo "USAGE: ./perf.sh numbers"
    exit;
fi
num=$1

for lists in 4 6 8 10 12
do
    ./radix $num $lists 1 | tee radix.single
    ./radix $num $lists $lists | tee radix.quad

    single=`grep 'Elapsed time:' radix.single | awk '{print $3}'`
    quad=`grep 'Elapsed time:' radix.quad | awk '{print $3}'`

    res=`echo "$quad > 0.0" | bc -l`

    if [ "$res" = "0" ]
    then
        speedup=0
    else
        speedup=`echo "scale=2; $single / $quad" | bc -l`
    fi

    printf "Speedup: %.3f times faster " $speedup

    required=`echo "$lists * 0.85" | bc -l`
    if (( $(echo "$speedup > $required" | bc -l) )) 
    then
        echo "PASS"
    else
        echo "FAIL"
    fi
done

for lists in 4 8 12 16 20 24
do
    (( cores = lists / 2 ))

    ./radix $num $lists $cores > radix.single &
    pid=$!

    cmax=0
    while true
    do
        cat /proc/$pid/status > radix.threads 2>/dev/null
        status=$?
        if [ $status -ne 0 ]
        then
            break
        fi

        grep "Generate time:" radix.single >/dev/null
        if [ $? -eq 0 ]
        then
            threads=`cat radix.threads | grep Threads | awk '{print $2'}`
            cmax=$(( threads > cmax ? threads : cmax ))
            echo -n .
        else
            echo -n -
        fi

        sleep 0.2
    done

    (( cmax -= 1 ))
    echo ""

    echo -n "Capped $lists lists $cores cores: "

    if [ $cmax -eq $cores ]
    then
        echo "PASS"
    else
        echo "FAIL"
    fi
done

rm radix.single radix.quad radix.threads 2>/dev/null
