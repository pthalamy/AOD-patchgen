#!/bin/bash

TIMEFORMAT="%U"

for bench in ./benchmark/benchmark*
do
    echo "./bin/computePatchOpt $bench/source $bench/target"

    for i in `seq 0 4`
    do
        t=$(time ./bin/computePatchOpt "$bench"/source "$bench"/target > /dev/null 2>1&)
	echo $t
    done
done
