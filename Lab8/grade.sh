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

echo ""
echo "################################################"
echo ""
echo "CMPS109 Spring 2018 Lab 8"
echo ""
date
echo ""
grep "Basic:" check.out
grep "Advanced:" check.out
grep "Stretch:" check.out
grep "Extreme:" check.out
echo ""

total=0;

pct=`cat check.out | grep 'Basic:' | grep '\%' | sed 's/\%//' | awk '{print $3}'`
tot=`echo "scale=2; $pct * 0.30" | bc -l`
printf "%20s: %4.1f/%2d ( %3.0f%% )\n" "Basic" $tot 30 $pct
total=`echo "scale=2; $total + $tot" | bc -l`

pct=`cat check.out | grep 'Advanced:' | grep '\%' | sed 's/\%//' | awk '{print $3}'`
tot=`echo "scale=2; $pct * 0.1" | bc -l`
printf "%20s: %4.1f/%2d ( %3.0f%% )\n" "Advanced" $tot 10 $pct
total=`echo "scale=2; $total + $tot" | bc -l`

pct=`cat check.out | grep 'Stretch:' | grep '\%' | sed 's/\%//' | awk '{print $3}'`
tot=`echo "scale=2; $pct * 0.1" | bc -l`
printf "%20s: %4.1f/%2d ( %3.0f%% )\n" "Stretch" $tot 10 $pct
total=`echo "scale=2; $total + $tot" | bc -l`

pct=`cat check.out | grep 'Extreme:' | grep '\%' | sed 's/\%//' | awk '{print $3}'`
tot=`echo "scale=2; $pct * 0.1" | bc -l`
printf "%20s: %4.1f/%2d ( %3.0f%% )\n" "Extreme" $tot 10 $pct
total=`echo "scale=2; $total + $tot" | bc -l`

ccode=0
if [ ! -s make.out ]
then
    (( ccode = 10 ))
fi

total=`echo "scale=2; $total + $ccode" | bc -l`
printf "%20s: %4.1f/%2d\n" "C++ Code" $ccode 10

printf "\n%20s: %4.1f/70\n\n" "Total" $total 

#rm check.out  2>/dev/null
