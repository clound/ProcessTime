#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/rtc.h>
#include <linux/moduleparam.h>

static int start_tm;
static int end_tm;
static struct task_struct* MyThread = NULL;
static struct task_struct *tmp_p = NULL;
static struct task_struct *end_p = NULL;
struct timeval tv;
struct timespec ts;
struct rtc_time tm;
static int irq,flag = 0;
static int flag1 = 0;
module_param(irq,int,0644);//用户向内核传参数的函数

static int start_time(void){
	get_monotonic_boottime(&ts);//获取系统启动以来所经过的时间，包含休眠时间
	do_gettimeofday(&tv);//得到1970到现在的秒数
	start_tm = tv.tv_sec - ts.tv_sec + tmp_p->start_time.tv_sec;//获得线程创建的时间
	rtc_time_to_tm(start_tm+8*3600,&tm);//转换为北京时间
	printk("The start time is :%d-%d-%d %d:%d:%d\n",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);	
	return (int)start_tm+8*3600;
}

static int end_time(void){
	do_gettimeofday(&tv);
	rtc_time_to_tm(tv.tv_sec+8*3600,&tm);
	printk("The end time is :%d-%d-%d %d:%d:%d\n",tm.tm_year+1900,tm.tm_mon+1,tm.tm_mday,tm.tm_hour,tm.tm_min,tm.tm_sec);	
	return (int)tv.tv_sec+8*3600;
}

static int MyPrintk(void *date){
	while(!kthread_should_stop()){//kthread_stop发送使线程退出的信号，而 kthread_should_stop则检查是否有退出信号，若有则使线程退出。
		end_p = NULL;
		for_each_process(tmp_p){//遍历所有进程
		if(tmp_p->pid == irq){//当传入的pid在其中找到
			end_p = tmp_p;			
			if(flag == 0){			
				start_tm = start_time();//调开始时间		
				flag = 1;
			}	
			break;	
		   }
		}
		if(end_p == NULL){
			if(flag1 == 0){	
				end_tm = end_time();//进程被kill掉，则调用退出时间		
				flag1 = 1;
			}		
		}	
		ssleep(2);
	}
	return 0;
}
static int __init
init_kthread(void)
{
	MyThread = kthread_run(MyPrintk,NULL,"mythread");//创建线程	
	return 0;
}
module_init(init_kthread);

static void __exit
exit_kthread(void)
{
	if(MyThread){
		printk("stop MyThread\n");
		kthread_stop(MyThread);//关闭线程
		printk("The thread survival time is %d\n",end_tm-start_tm);//线程创建和退出的时间差
	}
}
module_exit(exit_kthread);
MODULE_LICENSE("GPL");

