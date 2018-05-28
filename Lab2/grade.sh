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
echo "CMPS109 Spring 2018 Lab 2"
echo ""
date

./check.sh | tee check.out

ptotal=0
pcnt=0
for pct in `cat check.out | grep -v 'Tests' | grep '\%' | sed 's/\%//' | awk -F "/" '{print $2}' | awk -F " " '{print $2}'`
do
    ptotal=`echo "scale=2; $ptotal + $pct" | bc -l`
    (( pcnt += 1 ))
done

total=0;
if (( pcnt > 0 ))
then
    total=`echo "scale=2; ($ptotal / $pcnt) * 0.6" | bc -l`
fi
rm check.out

ccode=0
if [ ! -s make.out ]
then
    (( ccode = 10 ))
fi
total=`echo "scale=2; $total + $ccode" | bc -l`
printf "%20s:  %3d/%3d\n" "C-Code" $ccode 10

printf "\n%20s: %4.1f/70.0\n\n" "Total" $total 

