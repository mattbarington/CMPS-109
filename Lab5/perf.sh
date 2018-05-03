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

for lists in {4..8}
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

    required=`echo "$lists - 0.5" | bc -l`
    if (( $(echo "$speedup > $required" | bc -l) )) 
    then
        echo "PASS"
    else
        echo "FAIL"
    fi
done
