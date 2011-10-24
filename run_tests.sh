#/bin/sh

# Measure cached system call time
./syscall_time -t 1 -o cached.data -a -w  

# Measure uncached system call time
./run_syscall.sh

# Measure process creation time 
./syscall_time -t 2 -o creat.data 
