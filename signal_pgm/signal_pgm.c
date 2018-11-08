#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

int fib(int n)
{
	if( n <= 1)
	{
		return n;
	}
	else
	{
		return(fib(n - 2) + fib(n - 1));
	}
}

void ctrl_c_handler(int sig)
{
	char answer;

	printf("\n\n%s%d\n\n%s",
		"Interrupt recieved! Signal = ", sig,
		"Do you wish to (c)ontinue or (q)uit? ");
	scanf("%c", &answer);
	if( answer == 'c')
	{
		signal(SIGINT, ctrl_c_handler);
	}
	else
	{
		exit(1);
	}
}

int main(void)
{
	int i;

	signal(SIGINT, ctrl_c_handler);
	for(i = 0; i < 46; i++)
	{
		printf("fib(%d) = %d\n", i, fib(i));
	}
	return 0;
}
