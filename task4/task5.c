#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/rtc.h>
static int start_time;
static int end_time;
static struct task_struct* MyThread = NULL;

static int print(void){
	struct timeval tv;
	struct rtc_time tm;
	do_gettimeofday(&tv);//得到1970到现在的秒数
	rtc_time_to_tm(tv.tv_sec+8*3600,&tm);//转换北京时间
	printk("BeiJing time is :%d-%d-%d %d:%d:%d\n",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);
	return (int)tv.tv_sec;
}
static int MyPrintk(void *date){
	while(!kthread_should_stop()){//kthread_stop发送使线程退出的信号，而 kthread_should_stop则检查是否有退出信号，若有则使线程退出。
		//printk("hello kernel thread\n");
		print();		
		ssleep(2);
	}
	return 0;
}
static int __init
init_kthread(void)
{
	MyThread = kthread_run(MyPrintk,NULL,"mythread");//创建线程
	start_time = print();	
	return 0;
}
module_init(init_kthread);

static void __exit
exit_kthread(void)
{
	if(MyThread){
		printk("stop MyThread\n");
		kthread_stop(MyThread);//关闭线程
		end_time = print();
		printk("The thread survival time is %d\n",end_time-start_time);
	}
}
module_exit(exit_kthread);
MODULE_LICENSE("GPL");

