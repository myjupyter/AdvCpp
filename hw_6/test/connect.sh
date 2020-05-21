#!/bin/bash

python3 -c "print('a'*1_000)" > ./message

for i in $(seq 1000); do 
    cat message | nc localhost 8080 & 
done

sleep 10s

pkill nc
rm ./message
