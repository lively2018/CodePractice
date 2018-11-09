#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define LOG_MSG 1

#if LOG_MSG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

#define MAX_STR_LENGTH 320

typedef struct node{
    char * token;
    int length;
    struct node * next;
}node_t;

const char graph[16][9] = { 
	{'A', 'B', 'F', 'E', 'A', 'A', 'A', 'A', 'A'}, 
	{'B', 'A', 'E', 'F', 'G', 'C', 'B', 'B', 'B'}, 
	{'C', 'B', 'F', 'G', 'H', 'D', 'C', 'C', 'C'}, 
	{'D', 'C', 'G', 'H', 'D', 'D', 'D', 'D', 'D'}, 
	{'E', 'A', 'B', 'F', 'J', 'I', 'E', 'E', 'E'}, 
	{'F', 'B', 'A', 'E', 'I', 'J', 'K', 'G', 'C'}, 
	{'G', 'C', 'B', 'F', 'J', 'K', 'L', 'H', 'D'}, 
	{'H', 'D', 'C', 'G', 'K', 'L', 'H', 'H', 'H'}, 
	{'I', 'E', 'F', 'J', 'N', 'M', 'I', 'I', 'I'}, 
	{'J', 'F', 'E', 'I', 'M', 'N', 'O', 'K', 'G'}, 
	{'K', 'G', 'F', 'J', 'N', 'O', 'P', 'L', 'H'}, 
	{'L', 'H', 'G', 'K', 'O', 'P', 'L', 'L', 'L'}, 
	{'M', 'I', 'J', 'N', 'M', 'M', 'M', 'M', 'M'}, 
	{'N', 'M', 'I', 'J', 'K', 'O', 'N', 'N', 'N'}, 
	{'O', 'N', 'J', 'K', 'L', 'P', 'O', 'O', 'O'}, 
	{'P', 'O', 'K', 'L', 'P', 'P', 'P', 'P', 'P'}, 
};

char char_grid[17];

typedef struct linkedlist{
    node_t *head;
    node_t *tail;
}linkedlist_t;

linkedlist_t stringList;
linkedlist_t wordList;
linkedlist_t pathList[8];
FILE *pTempFile;

static node_t * createNode(char * newString, int length)
{
    node_t *list;
    list = malloc(sizeof(node_t));
    if(list == NULL)
    {
      printf("[%s]It fail to allocate memory.\n",__func__);
    }
    else
    {
        list->token = (char*)malloc(length+1);
        strcpy(list->token, newString);
        list->length = length;
        list->next = NULL;
    }
	return list;
}

static void insertNode(node_t *list, linkedlist_t *linkedlist)
{
    node_t *current;
    node_t *temp;
    /* The number of nodes is zero */
    if(linkedlist->head == NULL)
    {
        linkedlist->head = linkedlist->tail = list;
    }
    else 
    {
        current = linkedlist->head;
        do   
        {
            /* find the location to insert new node */
            if(strcmp(current->token, list->token) != 0)
            {
//				DEBUG_PRINT("current: %s list:%s \n", current->token, list->token);
                /* insert new node after tail */
                linkedlist->tail->next = list;
                linkedlist->tail = list;
                current = linkedlist->tail;
            }
			else
			{
//				DEBUG_PRINT("current: %s list:%s \n", current->token, list->token);
				free(list);
				current = linkedlist->tail;
			}
            current = current->next;
        }while(current != NULL);
    }
}

void print_list(linkedlist_t *pList)
{
	node_t *current;

	current = pList->head;
	do
	{
		if(current != NULL)
		{
			printf("%s\n", current->token);
		}
		current = current->next;
	}while(current != NULL);

}

void translate_from_way_to_word(char *way, char *word, int length)
{
	char one;
	int i;
	int index;

	for(i = 0; i < length; ++i)
	{
		one = way[i];
		index = one - 'A';
		*(word+i) = char_grid[index];
	}
}

void create_stringfile(int n)
{
    node_t *list = NULL;
	node_t *head = NULL;
	node_t *tail = NULL;
	node_t *current = NULL;
    char way[9];
	char word[9];
    int i; 

    for(current = pathList[n].head; current != NULL; current = current->next)
    {
		memset(way, 0x00, sizeof(way));
		memset(word, 0x00, sizeof(word));
		strcpy(way, current->token); 
		translate_from_way_to_word(way, word, strlen(way));
//		DEBUG_PRINT("[%s]way:%s word:%s\n", __FUNCTION__, way, word);
		fprintf(pTempFile, "%s\n",word);
    }
}
 
void make_string_list(void)
{
	int i;
	char newString[MAX_STR_LENGTH];
	int loop = 1;
	node_t *list;

	stringList.head = NULL;
	stringList.tail = NULL;

	pTempFile = fopen("tmp.txt","r");

	do
	{
		memset(newString, 0x00, sizeof(newString));
		if( fgets(newString, sizeof(newString), pTempFile) != NULL)
		{
			list = createNode(newString, strlen(newString-1));	
			insertNode(list, &stringList);
		}
		else
		{
			loop = 0;
		}
	}while(loop);
	
	fclose(pTempFile);
}   

void free_list(linkedlist_t *pList)
{
	node_t *current;
	node_t *list;

	current = pList->head; 
	while(current != NULL)
	{
		list = current;
		current = current->next;
		free(list);
	}
}
void create_path_set(int n)
{
	char str[9];
	char newstr[9];
	int i, j, k; 
	node_t *list = NULL;
	char last_point;
	node_t *current = NULL;


	if(n == 0)
	{
		for(i = 0; i < 16; ++i)
		{
			memset(str, 0x00, sizeof(str));
			*(str+n) =  graph[i][0];
			list = createNode(str, strlen(str));
			//DEBUG_PRINT("[%s]:token:%s\n", __FUNCTION__, list->token);
            insertNode(list, &pathList[n]);
		}
		create_stringfile(n);
	}
	else
	{
		//DEBUG_PRINT("[%s]n:%d\n", __FUNCTION__, n);
		for(current = pathList[n-1].head; current != NULL; current = current->next)
		{
		
			memset(str, 0x00, sizeof(str));
			strcpy(str, current->token);
		//	DEBUG_PRINT("[%s]str:%s\n", __FUNCTION__, str);
			last_point = *(str + n - 1);
			k = -1;
			for(i = 0; i < 16; ++i)
			{
				if(last_point == graph[i][0])	
				{
					k = i;
				}
			}
			assert(k != -1);
			//DEBUG_PRINT("[%s]k:%d\n", __FUNCTION__, k);
			for(i = 0; i < 9; ++i)
			{
				if( strchr(str, graph[k][i]) == NULL)
				{
					memset(newstr, 0x00, sizeof(newstr));
					strcpy(newstr, str);
					*(newstr + n) = graph[k][i];
					list = createNode(newstr, strlen(newstr));
					//DEBUG_PRINT("[%s]:token:%s\n", __FUNCTION__, list->token);
            		insertNode(list, &pathList[n]);
				}
			}
		}
		create_stringfile(n);
		free_list(&pathList[n-1]);
	}
}

void make_path_set(void)
{
	int i;
	node_t *current = NULL;
	node_t *list = NULL;

	pTempFile = fopen("tmp.txt","w");

	for(i = 0; i < 8; ++i)
	{
		create_path_set(i);
	}
	free_list(&pathList[i-1]);

	fclose(pTempFile);
}

int find_same_word(char *str)
{
	node_t *current;

	for(current = stringList.head; current != NULL; current = current->next)
	{
		if(strcmp(current->token, str) == 0)
		{
			//DEBUG_PRINT("[%s]:toke:%s str:%s\n", __FUNCTION__, current->token, str);
			return 0;
		}

	}
	return 1;
}

int main(int argc, char * argv[])
{
    FILE * fp = NULL;
    int loop;
	node_t *list;

    int newStringLength = 0;
    char newString[MAX_STR_LENGTH];

    /* Check the argument */
    if(argc != 3)
    {
        printf("Wrong arguments! It needs one argument as the file path.\n");
        return -1;
    }

    /* Check the file */
    fp = fopen(argv[1], "r");
    if(fp == NULL)
    {
        printf("%s doesn't exist.\n",argv[1]);
        return -1;
    }
    

    if( strlen(argv[2])!= 16)
    {
        printf("%s is wrong length\n", argv[2]);
        return -1; 
    }
    else
    {
		memset(char_grid, 0x00, sizeof(char_grid));
        strcpy(char_grid, argv[2]);
       
    }
	/* make path set */
	make_path_set();
	/* create string list */
	make_string_list();

	wordList.head = NULL;
	wordList.tail = NULL;
    /* Read a string from a file */
    loop = 1;
    do
    {
       /* fix the length of string to 320 bytes */
       memset(newString, 0x00, sizeof(newString));
       if(fgets(newString, MAX_STR_LENGTH, fp)!= NULL)
       {
          newStringLength = strlen(newString);
		  if(newStringLength < 8)
		  {
          	if(find_same_word(newString) == 0)
		  	{
	//   		     DEBUG_PRINT("%s", newString);
				 list = createNode(newString, newStringLength);
				 insertNode(list, &wordList);
		  	}
		 }
        }
        else
        {
            loop = 0;
        }
    }while(loop);
   
	free_list(&stringList);   	
    print_list(&wordList);
	free_list(&wordList);
	system("rm tmp.txt");

    return 0;
}

