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
echo "################################################"
echo ""
echo "CMPS109 Spring 2018 Lab 6"
echo ""
date

pass=`grep PASS check.out | wc -l`
fail=`grep FAIL check.out | wc -l`
(( tests = pass + fail ))

pct=`cat check.out | grep 'Tests' | grep '\%' | sed 's/\%//' | awk -F "/" '{print $2}' | awk -F " " '{print $2}'`
total=`echo "scale=2; $pct * 0.2" | bc -l`
printf "\n%20s: %4.1f/%2d ( %2d of %d tests )\n" "Functional" $total 20 $pass $tests

spass=`grep 'Speedup ' perf.out | grep PASS | wc -l`
sfail=`grep 'Speedup ' perf.out | grep FAIL | wc -l`
cpass=`grep Capped perf.out | grep PASS | wc -l`
cfail=`grep Capped perf.out | grep FAIL | wc -l`
(( pass = spass + cpass ))
(( fail = sfail + cfail ))
(( tests = pass + fail ))

ptotal=0
if [ $tests -ne 0 ]
then
    ptotal=`echo "scale=2; ($pass / $tests ) * 40.0" | bc -l`
fi

printf "%20s: %4.1f/%2d ( %2d of %d tests )\n" "Non-Functional" $ptotal 40 $pass $tests

ccode=0
if [ ! -s make.out ]
then
    (( ccode = 10 ))
fi

total=`echo "scale=2; $total + $ptotal + $ccode" | bc -l`
printf "%20s:  %3d/%2d\n" "C++ Code" $ccode 10

printf "\n%20s: %4.1f/70\n\n" "Total" $total 

rm perf.out check.out 2>/dev/null
