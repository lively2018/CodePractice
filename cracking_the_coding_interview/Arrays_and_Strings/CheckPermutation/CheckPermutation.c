#include <stdio.h>
#include <string.h>

int CheckPermutation(char *Source, char *Dest, int len, int n)
{
	char TempDest[128];
	char * position;
	int i, j;

	if(n <= 1)
	{
		if(strchr(Dest, *(Source+len-n)) != NULL)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		position = strchr(Dest, *(Source+len-n));
		if(position != NULL)
		{
			memset(TempDest, 0x00, sizeof(TempDest));
			strcpy(TempDest, Dest);
			for(j = 0, i = 0; i< strlen(Dest); i++,j++)
			{
				if(i != (position - TempDest))
				{
					*(Dest + j) = *(TempDest + i);
				}
				else
				{
					j--;
				}
			}
			n--;
			CheckPermutation(Source, Dest, len, n);
		}
		else
		{
			return 0;
		}
	}
}

int main(int argc, char *argv[])
{
	int SourceLength;
	int DestLength;

	if(argc != 3)
	{
		fprintf(stderr, "Wrong arguments\n Usage: CheckPermuation [SourceString] [DestString]\n");
		return -1;
	}

	SourceLength = strlen(argv[1]);
	DestLength = strlen(argv[2]);
	if(SourceLength > 128 || DestLength > 128)
	{
		fprintf(stderr, "Too long string, please enter 128 characters each a string.\n");
	}
	if(SourceLength == DestLength)
	{
	
		if(CheckPermutation(argv[1], argv[2], SourceLength, SourceLength))
		{
			printf("%s is permutation of %s\n", argv[2], argv[1]);
		}
		else
		{
			printf("%s isn't a permutation of %s\n", argv[2], argv[1]);
		}

	}
	else
	{
		printf("%s isn't a permutation of %s\n", argv[2], argv[1]);
	}
}
