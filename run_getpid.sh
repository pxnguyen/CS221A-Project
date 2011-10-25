#/bin/sh

for i in {1..10000}
do
  ./all_tests -t 0 -r 1 -n 1 -a -o getpid.data
done
