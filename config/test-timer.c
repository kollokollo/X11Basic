#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int count=0;
struct itimerval i = {{0,1},{0,1}};
struct itimerval v = {{1,0},{1,0}};

void handler ()
{
	count++;
#ifdef SYSV_SIGNALS
	signal (SIGALRM, handler);
#endif
}

int main ()
{
	int p, status;
	switch (p = fork())
	{
	case 0:
		signal (SIGALRM, handler);
		setitimer (ITIMER_REAL, &v, NULL);
		sigpause(0);
		break;
	default:
		signal (SIGALRM, handler);
		setitimer (ITIMER_REAL, &i, NULL);
		wait (&status);
		printf ("%d per second\n", count);
	}
	return 0;
}

