#include "BST.h"

static void RSD(struct Node* parent, FILE* fout)
{
	while (parent)
	{
		write_entry(fout, parent->entry);
		RSD(parent->ptr[1], fout);
		parent = parent->ptr[0];
	}
}

static void SRD(struct Node* parent, FILE* fout)
{
	while (parent)
	{
		SRD(parent->ptr[1], fout);
		write_entry(fout, parent->entry);
		parent = parent->ptr[0];
	}
}

static void SDR(struct Node* parent, FILE* fout)
{
	if (parent)
	{
		SDR(parent->ptr[1], fout);
		SDR(parent->ptr[0], fout);
		write_entry(fout, parent->entry);
	}
}

static void print(struct Node** data, FILE* fout)
{
	if (*data == NULL)
		return;
	
	printf("Ordinea parcurgerii:\n"
		"1. Preordine;\n"
		"2. Inordine;\n"
		"3. Postordine.\n");

	for (; ; )
	{
		putchar('>');
		
		size_t n = 0;
		if (scanf_s("%zu", &n) == 0)
            while (getchar() != '\n') {}

		switch (n)
		{
		case 1:
			RSD(*data, fout);
			return;

		case 2:
			SRD(*data, fout);
			return;

		case 3:
			SDR(*data, fout);
			return;

		default:
			puts("Optiune invalida.");
		}
	}
}

static void insert(struct Node** data, struct Entry* entry)
{
	struct Node* parent;

	if (*data == NULL)
		parent = *data = NewDuoNode();
	else for (parent = *data; ; )
	{
		if (entry->id > parent->entry->id)
		{
			if (parent->ptr[0] == NULL)
			{
				parent = parent->ptr[0] = NewDuoNode();
				break;
			}
			else
				parent = parent->ptr[0];
		}
		else
		{
			if (parent->ptr[1] == NULL)
			{
				parent = parent->ptr[1] = NewDuoNode();
				break;
			}
			else
				parent = parent->ptr[1];
		}
	}

	parent->entry = entry;
}

static struct Entry* find(struct Node** data)
{
	size_t id = SIZE_MAX;
	printf("WHERE id = ");
	scanf_s("%zu", &id);
	
	struct Node* parent = *data;

	while (parent)
	{
		if (id == parent->entry->id)
			return parent->entry;

		parent = parent->ptr[id < parent->entry->id];
	}

	return NULL;
}

static void erase(struct Node** data)
{
	size_t id = SIZE_MAX;
	printf("WHERE id = ");
	scanf_s("%zu", &id);
	
	while (*data)
	{
		if (id == (*data)->entry->id)
		{
			if ((*data)->ptr[0] == NULL && (*data)->ptr[1] == NULL)
				*data = NULL;
			else if ((*data)->ptr[0] == NULL)
				*data = (*data)->ptr[1];
			else if ((*data)->ptr[1] == NULL)
				*data = (*data)->ptr[0];
			else
			{
				struct Node** mnParent = &(*data)->ptr[0];

				while ((*mnParent)->ptr[1])
					mnParent = &(*mnParent)->ptr[1];

				struct Node* mnNode = *mnParent;

				*mnParent = (*mnParent)->ptr[0];

				mnNode->ptr[0] = (*data)->ptr[0];
				mnNode->ptr[1] = (*data)->ptr[1];
				*data = mnNode;
			}

			return;
		}

		data = &(*data)->ptr[id < (*data)->entry->id];
	}
}

void* InitBST(struct AbstractType* type)
{
	type->name = "Arbore binar de cautare";
	type->print = print;
	type->insert = insert;
	type->find = find;
	type->erase = erase;
	return cem_calloc(1, sizeof(struct Node*));
}