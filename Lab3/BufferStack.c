#include "BufferStack.h"

struct Stack
{
	struct Entry** ptr;
	size_t size, count;
};

static void insert(struct Stack* data, struct Entry* entry)
{
	if (++data->count > data->size)
		data->ptr = cem_realloc(data->ptr, sizeof(struct Entry*) * (data->size <<= 1));

	data->ptr[data->count - 1] = entry;
}

static struct Entry* find(struct Stack* data)
{
	return data->count == 0 ? NULL : data->ptr[data->count - 1];
}

static void erase(struct Stack* data)
{
	if (data->count != 0)
		--data->count;
}

static void print(struct Stack* data, FILE* fout)
{
	void* temp = find(data);

	if (temp == NULL)
		return;

	write_entry(fout, temp);
	erase(data);
	print(data, fout);
	insert(data, temp);
}

void* InitBufferStack(struct AbstractType* type)
{
	type->name = "Stiva implementata prin buffer";
	type->print = print;
	type->insert = insert;
	type->find = find;
	type->erase = erase;

	struct Stack* newStack = cem_calloc(1, sizeof(struct Stack));
	*newStack = (struct Stack){ cem_calloc(8, sizeof(struct Entry*)), 8 };

	return newStack;
}