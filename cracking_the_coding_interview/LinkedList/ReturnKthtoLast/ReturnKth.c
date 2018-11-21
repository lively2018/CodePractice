#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef struct node{
	char data;
	struct node *next;
}node_t;


void printlist(node_t *head)
{
	node_t *current;
	for(current = head; current != NULL; current = current->next)
	{
		printf("%c ", current->data);
	}
	printf("\n");
}
node_t *find_kth(int order, node_t *head)
{
	node_t *current;
	int total = 0;
	int reOrder;
	int i;

	for(current = head; current != NULL; current = current->next)
	{
		total++;
	}
	if( order > total)
	{
		printf("%dth element  doesn't exist.\n", order);
		return NULL;
	}
	else if(order == 0 || order == 1)
	{

		reOrder = total;
	}
	else
	{
		reOrder = total - order + 1;
	}

	for(i = 1, current = head; i < reOrder; i++)
	{
		current = current->next;	
	}

	return current;
}

int main(void)
{
	char data;
	node_t *element;
	node_t *head;
	node_t *current;
	node_t *dest;
	int order;

	head = NULL;
	do	
	{
		printf("Enter one character if 'q' , quick this progrma: ");
		scanf("%c", &data);
		getchar();
		element = malloc(sizeof(node_t));
		assert(element != NULL);
		element->next = NULL;
		element->data = data;
		if(head == NULL)
		{
			head = element;
		}
		else
		{
			current = head;
			while(current != NULL)	
			{
				if(current->next == NULL)
				{
					current->next = element;
					current = NULL;
				}
				else
				{
					current = current->next;	
				}
			}

		}

	}while( data != 'q');
	

	printf("Enter K: ");
	scanf("%d", &order);
	
	printlist(head);
	dest = find_kth(order, head);
	if(dest != NULL)
	{
		printf("%dth to the last element: %c\n", order, dest->data);
	}
	return 0;
}

