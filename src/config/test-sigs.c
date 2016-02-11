#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int count=0;
struct itimerval i = {{0,10000},{0,10000}};

void handler ()
{
	if (++count == 2) exit(0);	
}

int main ()
{
	signal (SIGVTALRM, handler);
	setitimer (ITIMER_VIRTUAL, &i, NULL);
	for(;;);
	return 0;
}

