#/bin/sh

for i in {1..10000}
do
  ./all_tests -t 6 -a -s -o kthread_oh.data
done
