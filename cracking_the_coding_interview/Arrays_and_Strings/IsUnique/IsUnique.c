#include <stdio.h>
#include <string.h>

int compare(char *str, int n)
{
	int i, j;
	for(i = 0; i < n-1; i++)
	{
		for(j = i+1; j < n; j++)
		{
			if(*(str+i) == *(str+j))
			{
				return 0;
			}
		}
	}

	return 1;
}

int main(int argc, char *argv[])
{
	int length;
	if(argc != 2)
	{
		fprintf(stderr,"Wrong arguments. Please enter a string.\n");
		return -1;
	}
	if(argv[1] == NULL)
	{
		fprintf(stderr,"Wrong arguments. Null string.\n");
		return -1;
	}

	length = strlen(argv[1]);
	if(length <= 1)
	{
		printf("%s is unique.\n", argv[1]);
		return 0;
	}
	if(compare(argv[1], length) != 0)
	{
		printf("%s is unique.\n", argv[1]);
	}
	else
	{
		printf("%s isn't unique.\n", argv[1]);
	}
	return 0;
}
