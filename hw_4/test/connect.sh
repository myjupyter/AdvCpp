#!/bin/bash

python3 -c "print('a'*10000)" > ./message

valgrind ../test.out &

for i in $(seq 4000); do 
    cat message | nc localhost 8080  & > /dev/null
done

pkill nc
rm ./message
