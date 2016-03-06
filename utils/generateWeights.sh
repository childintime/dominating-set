#!/bin/bash

if [ $# -ne 2 ]
  then
    echo "Need 2 arg: max-value number-of-values"
    exit 1
fi

for i in `seq -s' ' 1 $2`; do 
    echo $((( RANDOM % $1 )  + 1 ))
done
