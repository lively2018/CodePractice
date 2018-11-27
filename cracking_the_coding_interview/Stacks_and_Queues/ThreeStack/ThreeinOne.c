#include <stdio.h>
#define MAX_SIZE 72

typedef struct{
	int data[MAX_SIZE];
	int first_head;
	int second_head;
	int third_head;
}Stack;

Stack threeStack;

int push(int order, int data)
{
	int retVal = 1;
	int head = 0;

	switch(order)
	{
		case 1:
			head = threeStack.first_head;
			if(head == (MAX_SIZE/3 - 1))	
			{
				printf("first stack is full\n");
				retVal = 0;
			}
			else
			{
				threeStack.data[++head] = data;
				threeStack.first_head = head;
			}
			break;
		case 2:
			head = threeStack.second_head;
			if(head == (((MAX_SIZE/3)*2) - 1))	
			{
				printf("second stack is full\n");
				retVal = 0;
			}
			else
			{
				threeStack.data[++head] = data;
				threeStack.second_head = head;
			}
			break;
		case 3:
			head = threeStack.third_head;
			if(head == (MAX_SIZE - 1))	
			{
				printf("third stack is full\n");
				retVal = 0;
			}
			else
			{
				threeStack.data[++head] = data;
				threeStack.third_head = head;
			}
			break;
		default:
			printf("Wrong order number\n");
			retVal = 0;
			break;
			
	}
	return retVal; 
}

int pop(int order, int *data)
{
	int retVal = 1;
	int head = 0;

	switch(order)
	{
		case 1:
			head = threeStack.first_head;
			if(head == -1)	
			{
				printf("first stack is empty\n");
				retVal = 0;
			}
			else
			{
				*data = threeStack.data[head--];
				threeStack.first_head = head;
			}
			break;
		case 2:
			head = threeStack.second_head;
			if(head == ((MAX_SIZE/3) - 1))	
			{
				printf("second stack is empty\n");
				retVal = 0;
			}
			else
			{
				*data = threeStack.data[head--];
				threeStack.second_head = head;
			}
			break;
		case 3:
			head = threeStack.third_head;
			if(head == ((MAX_SIZE/3*2) - 1))	
			{
				printf("third stack is empty\n");
				retVal = 0;
			}
			else
			{
				*data = threeStack.data[head--];
				threeStack.third_head = head;
			}
			break;
		default:
			printf("Wrong order number\n");
			retVal = 0;
			break;
			
	}
	return retVal; 

}
int peer(int order, int *data)
{
	int retVal = 1;
	int head = 0;

	switch(order)
	{
		case 1:
			head = threeStack.first_head;
			if(head == - 1)	
			{
				printf("first stack is empty\n");
				retVal = 0;
			}
			else
			{
				*data = threeStack.data[head];
			}
			break;
		case 2:
			head = threeStack.second_head;
			if(head == (MAX_SIZE/3 - 1))	
			{
				printf("second stack is empty\n");
				retVal = 0;
			}
			else
			{
				*data = threeStack.data[head];
			}
			break;
		case 3:
			head = threeStack.third_head;
			if(head == (((MAX_SIZE/3)*2) - 1))	
			{
				printf("third stack is empty\n");
				retVal = 0;
			}
			else
			{
				*data = threeStack.data[head];
			}
			break;
		default:
			printf("Wrong order number\n");
			retVal = 0;
			break;
			
	}
	return retVal; 
}
void initialize(void)
{

	threeStack.first_head = -1;
	threeStack.second_head = MAX_SIZE/3 - 1;
	threeStack.third_head = ((MAX_SIZE/3)*2) - 1;
}

int main(void)
{
	int order, data;
	int top;
	int i;
	initialize();
	do	
	{
		printf("push stack order(if quick, order is 0): \n");
		scanf("%d", &order);
		if(order == 0)
			break;
		printf("push data: \n");
		scanf("%d", &data);
		push(order,data);
	}while(order != 0);

	for(i = 1; i < 4; i++)
	{
		if(peer(i, &top))
		{
			printf("top of 1st stack:%d\n",top); 
		}
	}

	do
	{
		printf("pop from stack order(if quick, order is 0): \n");
		scanf("%d", &order);
		if(order == 0)
			break;
		if(pop(order,&data))
		{
			printf("data:%d\n",data);
		}
	}while(order != 0);
	
	for(i = 1; i < 4; i++)
	{
		if(peer(i, &top))
		{
			printf("top of 1st stack:%d\n",top); 
		}
	}
	return 0;
}
