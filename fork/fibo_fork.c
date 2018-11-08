#include <stdio.h>
#include <time.h>

int fibo(int n)
{
	if(n <= 1)
	{
		return n; 
	}
	else
	{
		return (fibo(n-2) + fibo(n-1));
	}
}

int main(void)
{
	int i;
	long begin;

	begin = time(NULL);
	if(fork() == 0)
	{
		for(i = 0; i < 30; i++)
		{
			printf("fib(%2d) = %d\n", i, fibo(i));
		}
	}
	else
	{
		for(i = 0; i < 30; i++)
		{
			sleep(2);
			printf("elapsed time = %ld\n", time(NULL) - begin);
		}
	}
	return 0;
}
