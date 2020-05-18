#!/bin/bash

pushd .. &>/dev/null

RESULTS="$(./test/build/test)"

popd

FAILS=$(echo "$RESULTS" | grep FAIL | wc -l)
PASSES=$(echo "$RESULTS" | grep PASS | wc -l)
TOTALS=$((FAILS + PASSES))
PERC=$(bc <<< "scale=2; $PASSES/$TOTALS*100")

echo "$RESULTS" | GREP_COLOR='31' grep --color=always -A 1 'FAIL'

if (( $(echo "$PERC > 70" | bc -l ) )); then
    printf "\e[31m$FAILS\e[m Test(s) failed, \e[32m$PASSES\e[m Test(s) passed. \e[32m$PASSES/$TOTALS\e[m, \e[32m%0.2f%%\e[m\n" $PERC

elif (( $(echo "$PERC > 50" | bc -l ) )); then
    printf "\e[31m$FAILS\e[m Test(s) failed, \e[32m$PASSES\e[m Test(s) passed. \e[33m$PASSES/$TOTALS\e[m, \e[33m%0.2f%%\e[m\n" $PERC

else
    printf "\e[31m$FAILS\e[m Test(s) failed, \e[32m$PASSES\e[m Test(s) passed. \e[31m$PASSES/$TOTALS\e[m, \e[31m%0.2f%%\e[m\n" $PERC

fi