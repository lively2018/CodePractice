#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef enum AnimalType { 
	CAT = 0, 
	DOG
}AnimalType_t;

typedef struct Animal{
	AnimalType_t data;
	struct Animal * next;
}Animal_t;


typedef struct LinkedList{
	Animal_t *head;
	Animal_t *tail;
}LinkedList_t;

int isEmpty(LinkedList_t *list)
{
	if(list->head == NULL)
		return 1;
	else
		return 0;
}

void initialize(LinkedList_t *list)
{
	list->head = list->tail = NULL;
}

Animal_t *createAnimal(AnimalType_t data)
{
	Animal_t *item; 

	item = (Animal_t *)malloc(sizeof(Animal_t));
	assert(item != NULL);
	item->data = data;
	item->next = NULL;

	return item;
}
void enqueue(LinkedList_t *list, AnimalType_t data)
{
	Animal_t *current;
	current = createAnimal(data);
	if(isEmpty(list))
	{
		printf("list is empty\n");
		printf("enqueue %d\n", data);
		list->tail = current;
		list->head = current;
	}
	else
	{
		printf("enqueue %d\n", data);
		list->tail->next = current;
		list->tail = current;
	}
}

AnimalType_t dequeueAny(LinkedList_t *list)
{
	Animal_t *current;
	Animal_t *head;
	AnimalType_t data;

	head = list->head;
	assert(head != NULL);
	current = head;
	list->head = head->next;
	data = current->data;
	free(current);
	
	return data;
}

int dequeueDog(LinkedList_t *list)
{

	Animal_t *current;
	Animal_t *head;
	Animal_t *prev;
	AnimalType_t data;

	head = list->head;
	prev = NULL;
	assert(head != NULL);
	current = head;
	prev = current;		
	while( current != NULL)
	{
		if(current->data == DOG)
		{
			if(list->head == current)
			{
				if(list->tail == current)
				{
					list->head = NULL;
					list->tail = NULL;
				}
				else
				{
					list->head = current->next;
				}
				free(current);
			}
			else if(list->head != current && list->tail == current)
			{
				prev->next = NULL;
				list->tail = prev;
				free(current);
			}
			else
			{
				prev->next = current->next;
				free(current);
			}
			break;
		}
		prev = current;
		current = current->next;
	}
	if(current == NULL)
	{
		printf("There is no dog in a linked list\n");
		return 0;
	}
	return 1;
}

int dequeueCat(LinkedList_t *list)
{

	Animal_t *current;
	Animal_t *prev;
	AnimalType_t data;
	if(isEmpty(list))
	{
		printf("There is no cat and dog\n");
		return 0;
	}
	current = list->head;
	prev = current;		
	while( current != NULL)
	{
		if(current->data == CAT)
			break;
		prev = current;
		current = current->next;
	}
	if(current == NULL)
	{
		printf("There is no cat in a linked list\n");
		return 0;
	}
	else
	{
		if(list->head == current)
		{
			list->head = current->next;
			if(list->head == NULL)
				list->tail = NULL;
			free(current);
		}
		else if(list->tail == current)
		{
			prev->next = NULL;
			list->tail = prev;
			free(current);
		}
		else
		{
			prev->next = current->next;
			free(current);
		}
	}
	return 1;
}

void display(LinkedList_t *list)
{
	Animal_t *current;
	if(!isEmpty(list))
	{
		for(current = list->head; current != NULL; current = current->next)
		{
			printf("%d:%s, ", current->data,(current->data)?"Dog":"Cat");
		}
		printf("\n");
	}
	else
	{	
		printf("There is no cat and dog.\n");
	}
}
int main(void)
{
	int data;
	LinkedList_t animalList;
	initialize(&animalList);
	do
	{
		printf("Select Animal(0:Cat 1:Dog Others:Quit):");
		scanf("%d", &data);
		if( data == 1 || data == 0)
		{
			enqueue(&animalList,data);	
		}
	}while(data == 0 || data == 1);

	display(&animalList);
	if(!isEmpty(&animalList))
	{
		data = dequeueAny(&animalList);
		printf("dequeue %s\n", (data)?"Dog":"Cat");
	}
	else
	{
		printf("There is no cat and dog\n");
	}
	display(&animalList);
	if(!isEmpty(&animalList))
	{
		if(dequeueDog(&animalList))
		{
			printf("dequue Dog.\n");
		}
	}
	else
	{	
		printf("There is no cat and dog\n");
	}
	display(&animalList);
	if(!isEmpty(&animalList))
	{
		if(dequeueCat(&animalList))
		{
			printf("dequue Cat.\n");
		}
	}
	else
	{
		printf("There is no cat and dog\n");
	}
	display(&animalList);
	return 0;
}
