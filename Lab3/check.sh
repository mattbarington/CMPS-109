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
tests=0
passes=0
./bounds |  tee bounds.out
pass=`grep PASS bounds.out | wc -l`
fail=`grep FAIL bounds.out | wc -l`
(( total = pass + fail ))
(( tests += total ))
(( passes += pass ))
pct=0;
if (( total > 0 ))
then
    pct=`echo "scale=2; $pass / $total * 100.0" | bc -l`
fi
rm bounds.out
echo "" 

pct=0
if (( tests > 0 ))
then
    pct=`echo "scale=2; $passes / $tests * 100.0" | bc -l`
fi
printf "%20s:  %3d/%3d   %5.1f%%\n\n" "Tests" $passes $tests $pct
