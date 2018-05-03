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

tests=0
passes=0
> radix.out
for lists in {1..4}
do
    for cores in $(seq 1 $lists)
    do
        ./radix $num $lists $cores | tee radix.tmp.out
        cat radix.tmp.out >> radix.out
    done
done
rm radix.tmp.out

pass=`grep PASS radix.out | wc -l`
fail=`grep FAIL radix.out | wc -l`
(( total = pass + fail ))
(( tests += total ))
(( passes += pass ))
pct=0;
if (( total > 0 ))
then
    pct=`echo "scale=2; $pass / $total * 100.0" | bc -l`
fi
rm radix.out
echo "" 

pct=0
if (( tests > 0 ))
then
    pct=`echo "scale=2; $passes / $tests * 100.0" | bc -l`
fi
printf "%20s:  %3d/%3d   %5.1f%%\n\n" "Tests" $passes $tests $pct

