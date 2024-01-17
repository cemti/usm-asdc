#include "LinkedList.h"

struct Node* NewMonoNode(void)
{
	return cem_calloc(1, sizeof(struct Node) + sizeof(struct Node*));
}

static void print(struct Node** data, FILE* fout)
{
	for (struct Node* current = *data; current; current = current->ptr[0])
		write_entry(fout, current->entry);
}

static void circular_print(struct Node** data, FILE* fout)
{
	if (*data == NULL)
		return;

	size_t n = 0;
	printf("Nr. de iteratii: ");
	scanf_s("%zu", &n);

	for (struct Node* current = *data; n--; current = current->ptr[0])
		write_entry(fout, current->entry);
}

static void insert(struct Node** data, struct Entry* entry)
{
	struct Node* back;
	
	if (*data == NULL)
		back = *data = NewMonoNode();
	else
	{
		printf("Pozitia inserarii:\n"
			"1. La inceputul listei;\n"
			"2. La sfarsitul listei;\n"
			"3. Dupa un element dat.\n");

		for (; ; )
		{
			putchar('>');

			size_t n = 0;
			if (scanf_s("%zu", &n) == 0)
				while (getchar() != '\n') {}

			switch (n)
			{
			case 1:
				back = NewMonoNode();
				back->ptr[0] = *data;
				*data = back;
				goto endLoop;

			case 2:
				for (back = *data; back->ptr[0] && back->ptr[0] != *data; back = back->ptr[0]) {}
				back = back->ptr[0] = NewMonoNode();
				goto endLoop;

			case 3:
			{
				size_t id;
				printf("WHERE id = ");
				scanf_s("%zu", &id);

				for (back = *data; back->entry->id != id; back = back->ptr[0])
				{
					if (back->ptr[0] == NULL || back->ptr[0] == *data)
						return;
				}

				struct Node* newNode = NewMonoNode();
				newNode->ptr[0] = back->ptr[0];
				back = back->ptr[0] = newNode;

				goto endLoop;
			}

			default:
				puts("Optiune invalida.");
			}
		}
	}
endLoop:
	back->entry = entry;
}

static void circular_insert(struct Node** data, struct Entry* entry)
{
	struct Node* initial = *data;
	insert(data, entry);

	struct Node* it = initial ? initial : *data;
	for (; it->ptr[0] && it->ptr[0] != initial; it = it->ptr[0]) {}
	it->ptr[0] = *data;
}

static struct Entry* find(struct Node** data)
{
	if (*data == NULL)
		return NULL;
	
	size_t id = SIZE_MAX;
	printf("WHERE id = ");
	scanf_s("%zu", &id);
	
	for (struct Node* current = *data; current; current = current->ptr[0])
	{
		if (current->entry->id == id)
			return current->entry;

		if (current->ptr[0] == *data)
			return NULL;
	}

	return NULL;
}

static inline void generic_erase(struct Node** data, size_t id)
{
	for (struct Node* current = *data; current->ptr[0] && current->ptr[0] != *data; current = current->ptr[0])
	{
		if (current->ptr[0]->entry->id == id)
		{
			current->ptr[0] = current->ptr[0]->ptr[0];
			break;
		}
	}
}

static void erase(struct Node** data)
{
	if (*data == NULL)
		return;

	size_t id = SIZE_MAX;
	printf("WHERE id = ");
	scanf_s("%zu", &id);

	if ((*data)->entry->id == id)
		*data = (*data)->ptr[0];
	else
		generic_erase(data, id);
}

static void circular_erase(struct Node** data)
{
	if (*data == NULL)
		return;

	size_t id = SIZE_MAX;
	printf("WHERE id = ");
	scanf_s("%zu", &id);

	if ((*data)->entry->id == id)
	{
		if ((*data)->ptr[0] == *data)
			*data = NULL;
		else
		{
			struct Node* it = *data;
			for (; it->ptr[0] != *data; it = it->ptr[0]) {}
			*data = it->ptr[0] = (*data)->ptr[0];
		}
	}
	else
		generic_erase(data, id);
}

void* InitLinkedList(struct AbstractType* type)
{
	type->name = "Lista simplu inlantuita";
	type->print = print;
	type->insert = insert;
	type->find = find;
	type->erase = erase;
	return cem_calloc(1, sizeof(struct Node*));
}

void* InitCircularLinkedList(struct AbstractType* type)
{
	void* data = InitLinkedList(type);
	type->name = "Lista simplu inlantuita ciclic";
	type->print = circular_print;
	type->insert = circular_insert;
	type->erase = circular_erase;
	return data;
}