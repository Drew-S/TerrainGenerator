#!/bin/bash

NOPASS=true

if [ "$1" == "-p" ]; then
    NOPASS=false
fi

pushd .. &>/dev/null

RESULTS="$(./test/build/test)"

popd &>/dev/null

clear

FAILS=$(echo "$RESULTS" | grep FAIL | wc -l)
PASSES=$(echo "$RESULTS" | grep PASS | wc -l)
TOTALS=$((FAILS + PASSES))
PERC=$(bc <<< "scale=4; $PASSES/$TOTALS*100")

if test $NOPASS; then
    echo "$RESULTS" | GREP_COLOR='31' egrep --color=always 'FAIL|$' | GREP_COLOR='33' egrep --color=always 'QWARN|$' | egrep 'QWARN|FAIL|Loc'
else
    echo "$RESULTS" | GREP_COLOR='31' egrep --color=always 'FAIL|$' | GREP_COLOR='33' egrep --color=always 'QWARN|$' | GREP_COLOR='32' egrep --color=always 'PASS|$' | egrep 'PASS|QWARN|FAIL|Loc'
fi

printf "\e[31m$FAILS\e[m Test(s) failed, \e[32m$PASSES\e[m Test(s) passed. "

if (( $(echo "$PERC > 70" | bc -l ) )); then
    printf "\e[32m$PASSES\e[m/\e[32m$TOTALS\e[m, \e[32m%0.2f%%\e[m\n" $PERC

elif (( $(echo "$PERC > 50" | bc -l ) )); then
    printf "\e[33m$PASSES\e[m/\e[32m$TOTALS\e[m, \e[33m%0.2f%%\e[m\n" $PERC

else
    printf "\e[31m$PASSES\e[m/\e[32m$TOTALS\e[m, \e[31m%0.2f%%\e[m\n" $PERC

fi