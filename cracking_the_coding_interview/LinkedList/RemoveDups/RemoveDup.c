#include <stdio.h>
#include <stdlib.h>

typedef struct node{
	int data;
	struct node * next;
	struct node * prev;
}node_t;


typedef struct linkedlist{
	node_t * head;
	node_t * tail;
}linkedlist_t;

int find_duplicate(node_t *dest, linkedlist_t *list)
{
	node_t *current;
	node_t *prev;
	node_t *temp;

	if(dest == list->tail)
	{
		printf("dest is tail\n");
		return 1;
	}
	else
	{
	    //find duplicate
		current = dest;
		while(current->next != NULL)
		{
			prev = current;
			current = current->next;
			if(dest->data == current->data)
			{
				printf("prev:%d current:%d \n", prev->data, current->data);
				if(current == list->tail)
				{
					printf("current is tail\n");
					prev->next = NULL;
					list->tail = prev;
				}
				else
				{
					prev->next = current->next;
				}
				temp = current;
				current = prev;
				free(temp);
			}
		}
		printf("\n");
		dest = dest->next;
		if(dest == NULL)
		{
			return 1;
		}
		find_duplicate(dest, list);
	}
}
int main(void)
{
	int i;
	int data;
	linkedlist_t unsortedList;
	node_t *element;
	node_t *dest;
	
	unsortedList.head = NULL;
	unsortedList.tail = NULL;
	
	for(i = 0; i < 10; i++) 
	{
		printf("Enter a digit number: ");
		scanf("%d", &data);
		element = malloc(sizeof(node_t));
		element->data = data;
		element->next = NULL;
		element->prev = NULL;
		if(unsortedList.tail == NULL)
		{
			unsortedList.head = element;
			unsortedList.tail = unsortedList.head;
		}
		else
		{
			element->prev = unsortedList.tail;
			unsortedList.tail->next = element;
			unsortedList.tail = element;
		}
	}

	find_duplicate(unsortedList.head, &unsortedList);
	for(dest = unsortedList.head; dest != NULL; dest = dest->next)
	{
		printf("%d ",dest->data);
	}

	printf("\n");
	return 0;
}
