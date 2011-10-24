#/bin/sh

# Measure cached system call time
./all_tests -t 1 -o cached.data -a -w  

# Measure uncached system call time
./run_syscall.sh

# Measure process creation time 
./all_tests -t 2 -o creat.data 
