#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct node{
	char data;
	struct node *prev;
	struct node *next;
}node_t;

typedef struct linkedlist{
	node_t *head;
	node_t *tail;
}linkedlist_t;

void printList(node_t *head)
{
	node_t *current;

	for(current = head; current != NULL; current = current->next)
	{
		printf("%c",current->data);
	}
	printf("\n");
}

void deleteList(node_t *head)
{
	node_t *current;
	node_t *temp;
	current = head; 
	do
	{
		temp = current;
		current = current->next;
		free(temp);
	}while(current != NULL);

}
int isPalindrome(linkedlist_t *list)
{
	node_t *front;
	node_t *rear;
	int loop;
	int palindrome;

	front = list->head;
	rear = list->tail;
	loop = 1;
	palindrome = 1;
	do
	{
		if(front->data != rear->data)
		{
			loop = 0;	
			palindrome = 0;
		}
		else
		{
			if(front->next == rear->prev)
			{
				loop = 0;
			}
			else if(front->next == rear)
			{
				loop = 0;
			}
			else
			{
				front = front->next;
				rear = rear->prev;
			}

		}
	}while(loop);
	
	return palindrome;
}
int main(void)
{
	int i;
	char data;
	linkedlist_t dList;
	node_t *element;

	dList.head = dList.tail = NULL;

	while(1)	
	{
		printf("Enter one character if 'q', stop:");
		scanf("%c",&data);
		getchar();
		if(data == 'q')
			break;
		element = malloc(sizeof(node_t));
		assert(element != NULL);
		element->data = data;
		element->prev = NULL;
		element->next = NULL;
		if(dList.head == NULL)
		{
			dList.head = element;
			dList.tail = element;
		}
		else
		{
			dList.tail->next = element;
			element->prev = dList.tail;
			dList.tail = element;
		}
	}

	printList(dList.head);
	if(isPalindrome(&dList))
	{
		printf("Palindrome\n");
	}
	else
	{
		printf("Not Palindrome\n");
	}
	deleteList(dList.head);
	return 0;
}
