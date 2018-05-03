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
    echo "USAGE: ./grade.sh numbers"
    exit;
fi

num=$1

./check.sh $num | tee check.out
./perf.sh  $num | tee perf.out

echo ""
echo "CMPS109 Spring 2018 Lab 5"
echo ""
date

pass=`grep PASS check.out | wc -l`
fail=`grep FAIL check.out | wc -l`
(( tests = pass + fail ))

pct=`cat check.out | grep 'Tests' | grep '\%' | sed 's/\%//' | awk -F "/" '{print $2}' | awk -F " " '{print $2}'`
total=`echo "scale=2; $pct * 0.3" | bc -l`
printf "\n%20s: %4.1f/%2d ( %d tests )\n" "Functional" $total 30 $tests

pass=`grep Speedup perf.out | grep PASS | wc -l`
fail=`grep Speedup perf.out | grep FAIL | wc -l`
(( tests = pass + fail ))

ptotal=0
if [ $pass -ne 0 ]
then
    ptotal=`echo "scale=2; ($tests / $pass ) * 30.0" | bc -l`
fi

printf "%20s: %4.1f/%2d ( %2d tests )\n" "Non-Functional" $ptotal 30 $tests

ccode=0
if [ ! -s make.out ]
then
    (( ccode = 10 ))
fi

total=`echo "scale=2; $total + $ptotal + $ccode" | bc -l`
printf "%20s:  %3d/%2d\n" "C++ Code" $ccode 10

printf "\n%20s: %4.1f/70\n\n" "Total" $total 
