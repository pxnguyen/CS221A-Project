load getpidc.data;
[m, s] = rs(getpidc);
disp('pid cached :'), disp([m, s]);


load devnullc.data;
[m, s] = rs(devnullc);
disp('devnull cached :'), disp([m, s]);

load getpid.data;
[m, s] = rs(getpid);
disp('getpid :'), disp([m, s]);

load devnull.data;
[m, s] = rs(devnull);
disp('devnull :'), disp([m, s]);

load task.data;
[m, s] = rs(task);
disp('task creation :'), disp([m, s]);

load task_oh.data;
[m, s] = rs(task_oh);
disp('task pipe oh :'), disp([m, s]);

load ctx.data;
[m, s] = rs(ctx);
disp('ctx switch :'), disp([m, s]);

load kthread.data;
[m, s] = rs(kthread);
disp('kthread create :'), disp([m, s]);

load kthread_ctx.data;
[m, s] = rs(kthread_ctx);
disp('kthread ctx switch :'), disp([m, s]);

load kthread_oh.data;  
[m, s] = rs(kthread_oh);
disp('kthread ctx pipe oh :'), disp([m, s]);

