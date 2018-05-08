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

speedup() {
    num=$1
    lists=$2
    cores=$3
    pct=$4

    ./perf $num $lists $cores | tee radix.single

    req=`echo "10 * $pct" | bc -l`
    echo -n "Speedup lists: $lists cores: $cores required: $req% "

    fail=`grep FAIL radix.single | wc -l`
    if [[ fail -gt 0 ]]
    then
        echo ": FAIL"
    else
        speedup=`grep 'Speedup:' radix.single | awk '{print $2}' | sed 's/\%//'`

        spdup=`echo "10 * $lists * ($speedup / $cores)" | bc -l`
        printf "achieved: %.0f%% : " $spdup

        if (( $(echo "$spdup > $req" | bc -l) ))
        then
            echo "PASS"
        else
            echo "FAIL"
        fi
    fi

}

capped() {
    num=$1
    lists=$2
    cores=$3

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

    echo ""
    echo -n "Capped $lists lists $cores cores: "
    fail=`grep FAIL radix.single | wc -l`
    pass=`grep PASS radix.single | wc -l`

    if [[ fail -gt 0 ]]
    then
        echo "FAIL"
    else
        if [[ pass -gt 0 ]]
        then
            (( cmax -= 1 ))
            #echo "max $cmax cores $cores"

            if [ $cmax -le $cores ]
            then
                echo "PASS"
            else
                echo "FAIL"
            fi
        else
            echo "FAIL"
        fi
    fi
}

ranged() {
    num=$1
    pct=$2

    for lists in 1 2 
    do
        (( cores = lists * 10 ))
        speedup $num $lists $cores $pct
    done
}

for pct in 15 20 25 30 35 40 45 50
do
    ranged $num $pct
done

for lists in 1 2 4 
do
    (( cores = lists * 3 ))
    capped $num $lists $cores
done

for lists in 1 2 4 
do
    (( cores = lists * 6 ))
    capped $num $lists $cores
done

rm radix.single radix.quad radix.threads 2>/dev/null
