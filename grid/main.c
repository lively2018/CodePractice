#define LOG_MSG 1

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* USER DEFINE */
#define MAX_STR_LENGTH 320

#if LOG_MSG
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#endif

typedef struct node{
    char * token;
    int length;
    struct node * next;
}node_t;

const int dist_grid[4][4] = { {0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9, 10, 11}, {12, 13, 14, 15}};
char char_grid[4][4];

typedef struct linkedlist{
    node_t *head;
    node_t *tail;
    int nodeNum;
}linkedlist_t;

linkedlist_t distStringListOne;



int nodeNum = 0;

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
        linkedlist->nodeNum++;
    }
    else 
    {
        current = linkedlist->head;
        do   
        {
            /* find the location to insert new node */
            if(strcmp(current->token, list->token) != 0)
            {
				DEBUG_PRINT("current: %s list:%s \n", current->token, list->token);
                /* insert new node after tail */
                linkedlist->tail->next = list;
                linkedlist->tail = list;
                linkedlist->nodeNum++;
                current = linkedlist->tail;
            }
			else
			{
				DEBUG_PRINT("current: %s list:%s \n", current->token, list->token);
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
	do
	{
		current = pList->head;
		if(current != NULL)
		{
			printf("%s\n", current->token);
		}
		current = current->next;
	}while(current != NULL);

}
void create_distanceOne_stringList(void)
{
    node_t *list = NULL;
    char one[8];
    int i, j; 

	distStringListOne.head = NULL;
	distStringListOne.tail = NULL;
	distStringListOne.nodeNum = 0;


    for(i = 0; i < 4; ++i)
    {
        for(j = 0; j < 4; ++j)
        {
			memset(one, 0x00, sizeof(one));
            sprintf(one,"%c", char_grid[i][j]);
            list = createNode(one, strlen(one));
			DEBUG_PRINT("%s",list->token);
            insertNode(list, &distStringListOne);
        }
    }
//	print_list(&distStringListOne);
}
 
void make_string_list(void)
{
    create_distanceOne_stringList();
}   


int main(int argc, char * argv[])
{
    FILE * fp = NULL;
    int loop;
    char grid[4][4] ={0};
    int i, j;

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
        for(i = 0; i < 4; i++)
        {
            for(j = 0; j < 4; j++)
            {
                char_grid[i][j] = *(argv[2] + i*4 + j);
            }
        }
    }
    /* make string list */
    make_string_list();
    /* Read a string from a file */
    loop = 1;
#if 0    
    do
    {
       /* fix the length of string to 320 bytes */
       memset(newString, 0x00, sizeof(newString));
       if(fgets(newString, MAX_STR_LENGTH, fp)!= NULL)
       {
          DEBUG_PRINT("%s", newString);
          newStringLength = strlen(newString);
          //compare the grid_word_list
         
        }
        else
        {
            loop = 0;
        }
    }while(loop);
#endif    
    
    return 0;
}

