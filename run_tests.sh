#/bin/sh

#Cached  getpid()
./all_tests -t 0 -o getpidc.data
echo "Test 0 getpid cached done"

#Cached devnull
./all_tests -t 0 -o devnullc.data
echo "Test 1 devnull cached done"

#Uncached getpid
./run_getpid.sh
echo "Test 0 getpid done"

#Uncached dev null
./run_devnull.sh
echo "Test 1 devnull done"

#Task creation time
./all_tests -t 2 -o task.data
echo "Test 2 task creation done"

#Context switch time
./all_tests -t 3 -n 1000 -o ctx.data
echo "Test 3 ctx switch done"

#Kthread creation time
./all_tests -t 4 -o kthread.data
echo "Test 4 kthread creation done"

#Kthread ctx switch
./run_kthread_ctx.sh
echo "Test 5 kthread ctx done"

  
#Kthread ctx switch
./run_kthread_pipe_oh.sh
echo "Test 6 kthread pipe oh done"

#task ctx pipe oh
./all_tests -t 7 -n 1000 -o task_oh.data
echo "Test 7 ctx pipe oh done"
