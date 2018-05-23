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
    echo "USAGE: ./check.sh numbers"
    exit;
fi
num=$1

rm radix.out 2>/dev/null

check() {
    flags=$1
    tag=$2

    for lists in 1 2 4
    do
        ./radix $num $lists 12 $flags | tee radix.tmp.out
        cat radix.tmp.out >> radix.out
    done
    rm radix.tmp.out

    total=14
    pass=`grep PASS radix.out | wc -l`
    fail=`grep FAIL radix.out | wc -l`
    pct=0;
    pct=`echo "scale=2; $pass / $total * 100.0" | bc -l`
    rm radix.out
    echo "" 
    printf "%20s:  %3d/%d   %5.1f%%\n\n" $tag $pass $total $pct
}

check ""    "Basic"
check "-r"  "Advanced"
check "-l"  "Stretch"
check "-rl" "Extreme"

rm radix.out 2>/dev/null
