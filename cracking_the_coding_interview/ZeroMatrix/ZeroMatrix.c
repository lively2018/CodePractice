#include <stdio.h>
#define MAX_ROW 12
#define MAX_COLUMN 12

void SetZeroRow(int index, int (*matrix)[MAX_COLUMN], int row, int column)
{
	int i;
	int value;
	for(i = 0; i < column; i++)
	{
		matrix[index][i] = 0;
	}
}

void SetZeroColumn(int index, int (*matrix)[MAX_COLUMN], int row, int column)
{
	int i;
	for(i = 0; i < row; i++)
	{
		matrix[i][index] = 0;
	}
}
int main(void)
{

	int matrix[MAX_ROW][MAX_COLUMN] = {0};
	int zeromatrix[MAX_ROW][MAX_COLUMN] = {0};
	int row, column;
	int i, j;

	printf("Enter the size of row(0 < row <= 12):");
	scanf("%d",&row);
	if(row > 12 && row < 0)
	{
		printf("wrong row value\n");
		return -1;
	}

	printf("Enter the size of column(0 < column <= 12):");
	scanf("%d",&column);
	if(column > 12 && column < 0)
	{
		printf("wrong column value\n");
		return -1;
	}
	printf("row:%d, column:%d\n", row, column);
	for( i = 0; i < row; i++)
	{
		for(j = 0; j < column; j++)
		{
			printf("Element(%d,%d): ", i, j);
			scanf("%d", &matrix[i][j]);
			if(matrix[i][j] == 0)
			{
				zeromatrix[i][j] = 1;
			}
		}
	}

	for(i = 0; i < row; i++)
	{
		for(j = 0; j < column; j++)
		{
			if(zeromatrix[i][j])
			{
				SetZeroRow(i, matrix, row, column);
				SetZeroColumn(j, matrix, row, column);
			}
		}
	}
	for(i = 0; i < row; i++)
	{
		for(j = 0; j < column; j++)
		{
			printf("%d ", matrix[i][j]);
		}
		printf("\n");
	}
	return 0;
}
