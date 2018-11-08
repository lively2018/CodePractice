#include <stdio.h>
#include <unistd.h>
#include <assert.h>

#define N 3
#define M 4

int add_vector(int *v)
{
	int i, sum = 0;
	
	for(i = 0; i < M; ++i)
	{
		sum += *(v + i);	
	}
	return sum;
}

int main(void)
{
	int a[N][M] = {{1, 1, 1, 1}, {2, 2, 2, 2}, {3, 3, 3, 3}};
	int i, row_sum, sum, pd[2];
	sum = 0;

	assert(pipe(pd) != -1);
	for(i = 0; i < N; ++i)
	{
		if(fork() == 0)
		{
			row_sum = add_vector(a[i]);
			assert(write(pd[1], &row_sum, sizeof(int)) != -1);
			return;
		}
	}

	for(i = 0; i < N; ++i)
	{
		assert(read(pd[0], &row_sum, sizeof(int)) != -1);
		sum += row_sum;
	}
	printf("Sum of the array = %d\n", sum);
	return 0;
}
