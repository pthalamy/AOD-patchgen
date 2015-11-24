#!/bin/sh

DIR=$1

echo >> log_file
echo $DIR >> log_file 

(time ./bin/computePatchOpt $DIR/source $DIR/target) 2>> log_file
./bin/applyPatch P $DIR/source > G
diff -Z G $DIR/target > diff_show

if [ -s diff_show ]
then
	tput setaf 1; echo $DIR : [TEST FAILED]	
	cat diff_show 
else
	tput setaf 2; echo $DIR : [TEST PASSED]
fi

tput setaf 7
