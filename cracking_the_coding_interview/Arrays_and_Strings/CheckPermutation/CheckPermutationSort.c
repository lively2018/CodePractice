#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int comparep(const void *p, const void*q)
{
	const char *ip = (const char *)p;
	const char *iq = (const char *)q;
	return ( *(char *)ip - *(char *)iq);
}

int main(int argc, char *argv[])
{
	char Source[128];
	char Dest[128];
	int SourceLen;
	int DestLen;
	
	if(argc != 3)
	{
		fprintf(stderr, "Usage: %s <source string>  <dest string>\n", argv[0]);
		return -1;
	}

	SourceLen = strlen(argv[1]);
	DestLen = strlen(argv[2]);

	if(SourceLen > 128 || DestLen > 128)
	{
	
		fprintf(stderr, "<source string> and  <dest string> shouldn't be longer than 127 length.\n");
	}
	if(SourceLen != DestLen)
	{
		printf("%s isn't a permutation of %s\n", argv[2], argv[1]);
		return 0;
	}
	memset(Source, 0x00, sizeof(Source));
	memset(Dest, 0x00, sizeof(Dest));
	strcpy(Source, argv[1]);
	strcpy(Dest, argv[2]);
	qsort((void*)Source, SourceLen , sizeof(char), comparep);
	qsort((void*)Dest, DestLen, sizeof(char), comparep); 
	if(strcmp(Source, Dest)	 == 0)
	{
		printf("%s is a permutation of %s\n", argv[2], argv[1]);
	}
	else
	{
		printf("%s isn't a permutation of %s\n", argv[2], argv[1]);
	}



	return 0;
}
