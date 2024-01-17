#include "ListQueue.h"

static void insert(struct Node** data, struct Entry* entry)
{
	if (data[0] == NULL)
		data[1] = data[0] = NewMonoNode();
	else
		data[1] = data[1]->ptr[0] = NewMonoNode();

	data[1]->entry = entry;
}

static struct Entry* find(struct Node** data)
{
	if (data[0] == data[1])
		return data[0]->entry;
	
	printf("Obtinerea elementului:\n"
		"1. Primul element (cap);\n"
		"2. Ultimul element (coada).\n");

	for (; ; )
	{
		putchar('>');

		size_t n = 0;
		if (scanf_s("%zu", &n) == 0)
			while (getchar() != '\n') {}

		switch (n)
		{
		case 1: case 2:
			return data[n - 1]->entry;

		default:
			puts("Optiune invalida.");
		}
	}
}

static void erase(struct Node** data)
{
	if (data[0] == NULL)
		return;

	if (data[0] == data[1])
		data[0] = data[1] = NULL;
	else
		data[0] = data[0]->ptr[0];
}

static void print(struct Node** data, FILE* fout)
{
	if (data[0] == NULL)
		return;
	
	void* first = data[0]->entry, * temp = first;

	do
	{
		write_entry(fout, temp);
		erase(data);
		insert(data, temp);

		temp = data[0]->entry;
	} while (temp != first);
}

void* InitListQueue(struct AbstractType* type)
{
	type->name = "Coada implementata prin lista simplu inlantuita";
	type->print = print;
	type->insert = insert;
	type->find = find;
	type->erase = erase;
	return cem_calloc(2, sizeof(struct Node*));
	// [0] - cap, [1] - coada
}