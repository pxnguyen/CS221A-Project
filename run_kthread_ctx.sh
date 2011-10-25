#/bin/sh

for i in {1..10000}
do
  ./all_tests -t 5 -a -s -o kthread_ctx.data
done
