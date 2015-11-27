#!/bin/sh

DIR=$1

./bin/computePatchOpt $DIR/source $DIR/target > $DIR/patchCompute
./bin/applyPatch $DIR/patchCompute $DIR/source > $DIR/sourcePatched
diff $DIR/sourcePatched $DIR/target > $DIR/diffShow

if [ -s $DIR/diffShow ]
then
    tput setaf 1; echo $DIR : [TEST FAILED]	
    cat diff_show 
else
    tput setaf 2; echo $DIR : [TEST PASSED]
    tput setaf 7; cat costPatch
fi
tput setaf 7

rm -f $DIR/patchCompute $DIR/sourcePatched $DIR/diffShow costPatch
