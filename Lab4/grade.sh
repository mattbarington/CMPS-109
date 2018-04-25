#!/bin/bash

#######################################################################
#
# Copyright (C) 2018 David C. Harrison. All right reserved.
#
# You may not use, distribute, publish, or modify this code without 
# the express written permission of the copyright holder.
#
#######################################################################

echo ""
echo "CMPS109 Spring 2018 Lab 3"
echo ""
date

./check.sh > check.out

pass=`grep PASS check.out | wc -l`
fail=`grep FAIL check.out | wc -l`
(( tests = pass + fails ))

pct=`cat check.out | grep 'Tests' | grep '\%' | sed 's/\%//' | awk -F "/" '{print $2}' | awk -F " " '{print $2}'`
total=`echo "scale=2; $pct * 0.6" | bc -l`
#rm check.out
printf "\n%20s: %4.1f/%2d ( %d tests )\n" "Tests" $total 60 $tests

ccode=0
if [ ! -s make.out ]
then
    (( ccode = 10 ))
fi

total=`echo "scale=2; $total + $ccode" | bc -l`
printf "%20s:  %3d/%2d\n" "C++ Code" $ccode 10

printf "\n%20s: %4.1f/70\n\n" "Total" $total 

