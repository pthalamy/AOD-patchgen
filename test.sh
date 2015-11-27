#!/bin/sh

for DIR in ./benchmark/benchmark*
do
    tput setaf 7
    ./bin/computePatchOpt $DIR/source $DIR/target > $DIR/patchCompute
    ./bin/applyPatch $DIR/patchCompute $DIR/source > $DIR/sourcePatched
    diff $DIR/sourcePatched $DIR/target > $DIR/diffShow

    if [ -s $DIR/diffShow ]
    then
	tput setaf 1; echo $DIR : [TEST FAILED]	
	cat $DIR/diffShow 
    else
	tput setaf 2; echo $DIR : [TEST PASSED]
	tput setaf 7; cat costPatch
    fi
    echo

    rm -f $DIR/patchCompute $DIR/sourcePatched $DIR/diffShow
done

tput setaf 7;
