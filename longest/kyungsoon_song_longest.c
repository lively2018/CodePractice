#define LOG_MSG 0

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
    struct node * prev;
}node_t;

node_t * head;
node_t * tail;
node_t * list;
int nodeNum = 0;

static int createNode(char * newString, int length)
{
    char * string;
    list = malloc(sizeof(node_t));
    if(list == NULL)
    {
      printf("[%s]It fail to allocate memory.\n",__func__);
      return 0;
    }
    else
    {
        list->token = malloc(length+1);
        strcpy(list->token, newString);
        list->length = length;
        list-> next = NULL;
        list-> prev = NULL;
    }
	return 1;
}

static void insertNode(void)
{
    node_t *current;
    node_t *temp;
    
    /* The number of nodes is zero */
    if(head == NULL)
    {
        head = tail = list;
        nodeNum++;
    }
    /* The number of nodes is under 10 */
    else if(nodeNum < 10)
    {
       
        current = head;
        do
        {
            /* find the location to insert new node */
            if(current->length < list-> length)
            {
                /* insert new node before head */
                if(current->prev == NULL)
                {
                    current->prev = list;
                    list->next = current;
                    head = list;
                }
                /* insert new node between head and tail */
                else
                {
                    temp = current->prev;
                    temp->next = list;
                    list->next = current;
                    list->prev = temp;
                    current->prev = list;
                }
                current = NULL;
                nodeNum++;
            }
            else
            {
                current = current->next;
                /* insert new node after tail */
                if(current == NULL)
                {
                    tail->next = list;
                    list->prev = tail;
                    tail = list;
                    nodeNum++;
                }
            }
        }while(current != NULL);
    }
    else
    {
        current = head;
        do
        {
            if(current->length < list-> length)
            {
                
                if(current->prev == NULL)
                {
                    current->prev = list;
                    list->next = current;
                    head = list;

                }
                else
                {
                    temp = current->prev;
                    temp->next = list;
                    list->next = current;
                    list->prev = temp;
                    current->prev = list;
                }
                temp = tail->prev;
                temp->next = NULL;
                /* remove old tail */
                current = tail;
                tail = temp;
                free(current);
                current = NULL;
            }
            else
            {
                current = current->next;
            }
        }while(current != NULL);       
    }
}

static void printNode(void)
{
    node_t * current;
    current = head;
    while(current != NULL)
    {
        printf("%s", current->token);
        current = current->next;
    }
}

int main(int argc, char * argv[])
{
    FILE * fp = NULL;
    int loop;

    int newStringLength = 0;
    char newString[MAX_STR_LENGTH];

    /* Check the argument */
    if(argc != 2)
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
    /* Read a string from a file */
    loop = 1;
    do
    {
       /* fix the length of string to 320 bytes */
       memset(newString, 0x00, sizeof(newString));
       if(fgets(newString, MAX_STR_LENGTH, fp)!= NULL)
       {
          DEBUG_PRINT("%s", newString);
          newStringLength = strlen(newString);

          if(nodeNum < 10)
          {
             if(!createNode(newString, newStringLength))
             {
                 return -1;
             }
             insertNode();
          }
          else  
          {
            if(tail->length < newStringLength)
            {
                if(!createNode(newString, newStringLength))
                {
                    return -1;
                }
                insertNode();
            }
          }
        }
        else
        {
            loop = 0;
        }
    }while(loop);
    
    printNode();
    return 0;
}
