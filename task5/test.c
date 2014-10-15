#include <stdio.h>
#include <unistd.h>
int main()
{
	pid_t pid = getpid();
		printf("pid is %d\n",pid);	
	while(1){
	}
	return 0;
}
