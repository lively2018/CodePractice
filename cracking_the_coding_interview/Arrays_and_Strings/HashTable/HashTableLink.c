#include <stdio.h>
#include <stdlib.h>

#define SIZE 20

typedef struct DataItem{
	int data;
	int key;
}DataItem_t;

typedef struct HashTable{
	int size;
	int count;
	DataItem_t *items;
}HashTable_t;

static DataItem_t * CreateNewDataItem(int data, int key)
{
	DataItem_t *item = (DataItem_t *) malloc((sizeof(DataItem_t));
	item->data =  data;
	item->key = key;

	return item;
}
static int hashCode(int size, int key)
{
	return key % size;	
}

static int insertItem(HashTable_t *hTable, int data, int key)
{
	DataItem_t *item = CreateNewItem(data, key);
	int index = hasCode(hTable->size, key);
	DataItem_t *cur_item = hTable->items[index];

	if(hTable->count != hTable->size)
	{
		while(item != NULL)
		{
			index++;
			item = hTable->items[index];
		}

		hTable->items[index] = item;
		hTable->count++;

		return 1;
	}
	else
	{
		return 0;
	}
	
}
HashTable_t * CreateNewHashTable(int size)
{
	HashTable_t *hTable = (HashTable_t *)malloc(sizeof(HashTable_t));

	hTable->size = size;
	hTable->count = 0;
	hTable->items = calloc((size_t)hTable->size,sizeof(DataItem_t *));

	return hTable;
}



void display(HashTable_t *hTable) {
	int i = 0;
	DataItem_t *item;

	for(i = 0; i < hTable->size; i++)
	{
		item = hTable->items[i];
		if(item != NULL)
		{
			printf(" (%d,%d))", item->key, item->data);
		}
		else
		{
			printf(" ~~");
		}
	}
	printf("\n");
}

int main(void)
{
	struct DataItem *item;

	insert(1, 20);
	insert(2, 70);
	insert(42, 80);
	insert(4, 25);
	insert(12, 44);
	insert(14, 32);
	insert(17, 11);
	insert(13, 78);
	insert(37, 97);

	display();
	item = search(37);

	if(item != NULL)
	{
		printf("Element found: %d\n", item->data);
	}
	else
	{
		printf("Element not found\n");
	}

	delete(item);
	item = search(37);

	if(item != NULL)
	{
		printf("Element found: %d\n", item->data);
	}
	else
	{
		printf("Element not found\n");
	}

	return 0;
}
