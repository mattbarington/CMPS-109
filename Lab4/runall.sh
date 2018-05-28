for f in tests/*; do ./bounds $f; done > log
if [[ $* == *-v* ]] 
then 
    cat log 
fi
total=$(grep -c "" log)
passed=$(grep -c "PASS" log)
printf "\nPassed $passed / $total Tests\n\n"

rm log