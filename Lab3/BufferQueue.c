#include "BufferQueue.h"

struct Queue
{
	struct Entry** ptr;
	size_t size, count, head_pos;
};

static void insert(struct Queue* data, struct Entry* entry)
{
	if (++data->count > data->size)
		data->ptr = cem_realloc(data->ptr, sizeof(struct Entry*) * (data->size <<= 1));

	size_t pos = data->head_pos + data->count - 1;

	if (pos >= data->size)
		pos -= data->size;

	data->ptr[pos] = entry;
}

static struct Entry* find(struct Queue* data)
{
	if (data->count == 0)
		return NULL;

	size_t pos = data->head_pos;

	if (data->count == 1)
		return data->ptr[pos];

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
		case 2:
			pos += data->count - 1;

		case 1:
			goto endLoop;

		default:
			puts("Optiune invalida.");
		}
	}
endLoop:

	if (pos >= data->size)
		pos -= data->size;

	return data->ptr[pos];
}

static void erase(struct Queue* data)
{
	if (data->count == 0)
		return;

	--data->count;

	if (++data->head_pos == data->size)
		data->head_pos = 0;
}

static void print(struct Queue* data, FILE* fout)
{
	if (data->count == 0)
		return;

	void* first = data->ptr[data->head_pos], * temp = first;

	do
	{
		write_entry(fout, temp);
		erase(data);
		insert(data, temp);

		temp = data->ptr[data->head_pos];
	} while (temp != first);
}

void* InitBufferQueue(struct AbstractType* type)
{
	type->name = "Coada implementata prin buffer";
	type->print = print;
	type->insert = insert;
	type->find = find;
	type->erase = erase;

	struct Queue* newQueue = cem_calloc(1, sizeof(struct Queue));
	*newQueue = (struct Queue){ cem_calloc(8, sizeof(struct Entry*)), 8 };

	return newQueue;
}