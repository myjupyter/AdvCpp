#!/bin/bash

python3 -c "print('a'*10000)" > ./message

valgrind ../test.out &

for i in $(seq 4024); do 
    cat message | nc localhost 8080 > /dev/null &
done

sleep 10

pkill nc
rm ./message
