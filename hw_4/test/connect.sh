#!/bin/bash

python3 -c "print('a'*1000000)" > ./message

../server.out &

for i in $(seq 4024); do 
    cat message | nc localhost 8080 > /dev/null &
done

sleep 10

pkill nc
rm ./message
