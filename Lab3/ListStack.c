#include "ListStack.h"

static void insert(struct Node** data, struct Entry* entry)
{
	struct Node* newNode = NewMonoNode();
	newNode->ptr[0] = *data;
	newNode->entry = entry;
	*data = newNode;
}

static struct Entry* find(struct Node** data)
{
	if (*data == NULL)
		return NULL;

	return (*data)->entry;
}

static void erase(struct Node** data)
{
	if (*data)
		*data = (*data)->ptr[0];
}

static void print(struct Node** data, FILE* fout)
{
	void* temp = find(data);

	if (temp == NULL)
		return;

	write_entry(fout, temp);
	erase(data);
	print(data, fout);
	insert(data, temp);
}

void* InitListStack(struct AbstractType* type)
{
	type->name = "Stiva implementata prin lista simplu inlantuita";
	type->print = print;
	type->insert = insert;
	type->find = find;
	type->erase = erase;
	return cem_calloc(1, sizeof(struct Node*));
}