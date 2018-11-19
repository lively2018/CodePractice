#include <stdio.h>
#include <string.h>

int choosechar(char *Source, int *Letter, int len)
{
	int i, j;

	for(i = 0; i< len; i++)
	{
		Letter[*(Source + i)]--;
		if(Letter[*(Source + i)] < 0)
		{
			return 0;
		}
	}
	return 1;
}

int main(int argc, char *argv[])
{
	int letters[128] = {0};
	int SrcLen;
	int DestLen;
	int i;

	if(argc != 3)
	{
		fprintf(stderr, "Usage: %s <source string> <dest string>\n", argv[0]);
		return -1;
	}
	
	SrcLen = strlen(argv[1]);
	DestLen = strlen(argv[2]);

	if(SrcLen != DestLen)
	{
		printf("%s isn't a permutation of %s\n", argv[2], argv[1]);
		return 0;
	}
	
	for(i = 0; i < SrcLen; i++)
	{
		letters[*(argv[1] + i)]++;
	}

	if(choosechar(argv[2], letters, DestLen))
	{
		printf("%s is a permutation of %s\n", argv[2], argv[1]);
	}
	else
	{
		printf("%s isn't a permutation of %s\n", argv[2], argv[1]);
	}

}
