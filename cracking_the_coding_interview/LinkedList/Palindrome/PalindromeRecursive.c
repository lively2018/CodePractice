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
	while(current->next != NULL)
	{
	
	}
}
int isPalindrome(node_t *front, node_t *rear)
{
	if(front == rear)
	{
		if(front->data == rear->data)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if(front->next == rear)
	{
		if(rear->next == NULL)
		{
			if(front->data == rear->data)
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
			return 1;
		}
	}
	else
	{
		if(front->data == rear->data)
		{
			front = front->next;
			rear = rear->prev;
			isPalindrome(front,rear);
		}
		else
		{
			return 0;
		}
	}
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
	if(isPalindrome(dList.head, dList.tail))
	{
		printf("Palindrome\n");
	}
	else
	{
		printf("Not Palindrome\n");
	}
	return 0;
}
