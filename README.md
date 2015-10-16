# ThreadTime
####线程时间
关键函数:
```
MyThread = kthread_run(MyPrintk,NULL,"mythread");//创建线程	
kthread_stop(MyThread);//关闭线程
while(!kthread_should_stop()){//kthread_stop发送使线程退出的信号，而 kthread_should_stop则检查是否有退出信号，若有则使线程退出。
get_monotonic_boottime(&ts);//获取系统启动以来所经过的时间，包含休眠时间
do_gettimeofday(&tv);//得到1970到现在的秒数
start_tm = tv.tv_sec - ts.tv_sec + tmp_p->start_time.tv_sec;//获得线程创建的时间
rtc_time_to_tm(start_tm+8*3600,&tm);//转换为北京时间
