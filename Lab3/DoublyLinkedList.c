#include "DoublyLinkedList.h"

struct Node* NewDuoNode(void)
{
	return cem_calloc(1, sizeof(struct Node) + (sizeof(struct Node*) << 1));
}

static void circular_print(struct Node** data, FILE* fout)
{
	if (*data == NULL)
		return;

	int64_t n = 0;
	printf("Nr. de iteratii: ");
	scanf_s("%lld", &n);

	bool isNegative = n < 0;

	for (struct Node* current = *data; isNegative ? n++ : n--; current = current->ptr[isNegative])
		write_entry(fout, current->entry);
}

static void insert(struct Node** data, struct Entry* entry)
{
	struct Node* back;

	if (*data == NULL)
		back = *data = NewDuoNode();
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
				back = NewDuoNode();
				back->ptr[0] = *data;
				(*data)->ptr[1] = back;
				*data = back;
				goto endLoop;

			case 2:
			{
				for (back = *data; back->ptr[0] && back->ptr[0] != *data; back = back->ptr[0]) {}

				struct Node* newNode = NewDuoNode();
				newNode->ptr[1] = back;
				back = back->ptr[0] = newNode;
				goto endLoop;
			}

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

				struct Node* newNode = NewDuoNode();
				newNode->ptr[0] = back->ptr[0];
				newNode->ptr[1] = back;

				if (back->ptr[0])
					back->ptr[0]->ptr[1] = newNode;

				back->ptr[0] = newNode;
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
	(*data)->ptr[1] = it;
	it->ptr[0] = *data;
}

static inline void generic_erase(struct Node** data, size_t id)
{
	for (struct Node* current = *data; current; current = current->ptr[0])
	{
		if (current->entry->id == id)
		{
			current->ptr[1]->ptr[0] = current->ptr[0];

			if (current->ptr[0])
				current->ptr[0]->ptr[1] = current->ptr[1];

			break;
		}
	}
}

static void erase(struct Node** data)
{
	if (*data == NULL)
		return;

	size_t id;
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

	size_t id;
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

			if (it->ptr[1] == *data)
				it->ptr[1] = (*data)->ptr[1];

			*data = it->ptr[0] = (*data)->ptr[0];
		}
	}
	else
		generic_erase(data, id);
}

void* InitDoublyLinkedList(struct AbstractType* type)
{
	void* data = InitLinkedList(type);
	type->name = "Lista dublu inlantuita";
	type->insert = insert;
	type->erase = erase;
	return data;
}

void* InitCircularDoublyLinkedList(struct AbstractType* type)
{
	void* data = InitDoublyLinkedList(type);
	type->name = "Lista dublu inlantuita ciclic";
	type->print = circular_print;
	type->insert = circular_insert;
	type->erase = circular_erase;
	return data;
}