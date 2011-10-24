#/bin/sh

for i in {1..10000}
do
  ./syscall_time -r 1 -n 1 -a 
done
