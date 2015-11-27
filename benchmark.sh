#!/bin/bash

TIMEFORMAT="%R"

for bench in ./benchmark/benchmark*
do
    echo "./bin/computePatchOpt $bench/source $bench/target"

    for i in `seq 0 4`
    do
        t=$(time ./bin/computePatchOpt "$bench"/source "$bench"/target > /dev/null)
    done
done
