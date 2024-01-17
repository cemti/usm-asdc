#include "database.h"

void write_entry(FILE* fout, struct Entry* entry)
{
	fprintf(fout, "%zu,%s,%s,%s,%c,%f,%hhu.%hhu.%hhu.%hhu,%s\n",
		entry->id, entry->firstName, entry->secondName, entry->email, entry->female ? 'F' : 'M',
		entry->tax, entry->ip[0], entry->ip[1], entry->ip[2], entry->ip[3], entry->country);
}

void remove_node(struct Node* node)
{
	if (node->prev->next)
		node->prev->next = node->next;

	if (node->prev->prev)
		node->next->prev = node->prev;

	free(node->entry);
	free(node);
}

struct Node* read_db(FILE* fin, size_t* count)
{
	struct Node* head = NULL, * current = NULL;
	size_t i = 0;

	for (; ; )
	{
		size_t id;
		fscanf_s(fin, "%zu,", &id);

		if (feof(fin))
			break;

		++i;

		struct Entry* entry = calloc(1, sizeof(struct Entry));

		if (entry == NULL)
			break;

		entry->id = id;

		{
			struct Node* temp = calloc(1, sizeof(struct Node));

			if (temp == NULL)
				break;

			if (head == NULL)
			{
				current = head = temp;
				temp->entry = entry;
			}
			else
			{
				temp->entry = entry;
				temp->prev = current;

				current->next = temp;
				current = temp;
			}
		}

		fscanf_s(fin, "%[^,],%[^,],%[^,],",
			entry->firstName, 50, entry->secondName, 50, entry->email, 100);

		{
			char g;
			fscanf_s(fin, "%c,", &g, 1);
			entry->female = (g | ' ') == 'f';
		}

		fscanf_s(fin, "%lf,%hhu.%hhu.%hhu.%hhu,%s",
			&entry->tax, &entry->ip[0], &entry->ip[1], &entry->ip[2], &entry->ip[3], entry->country, 3);
	}

	if (count)
		*count = i;
	
	return head;
}

void write_db(FILE* fout, struct Node* head)
{
	for (struct Node* it = head; it; it = it->next)
		write_entry(fout, it->entry);
}

void free_db(struct Node* head)
{
	for (struct Node* it = head; it; )
	{
		free(it->entry);
		void* next = it->next;
		free(it);
		it = next;
	}
}

void shuffle_db(struct Node* head)
{
	for (struct Node* it = head; it; )
	{
		struct Node* temp = it->next;

		if (temp == NULL)
			return;

		void* ptr = it->entry;
		it->entry = temp->entry;
		temp->entry = ptr;

		it = temp->next;
	}
}

static struct CemNode
{
	void* data;
	struct CemNode* next;
} *back, *front;

void* cem_calloc(size_t count, size_t size)
{
	if (front)
	{
		front->next = calloc(1, sizeof(struct CemNode));
		front->next->data = calloc(count, size);
		front = front->next;
		return front->data;
	}

	back = front = calloc(1, sizeof(struct CemNode));
	back->data = calloc(count, size);
	return back->data;
}

void cem_collect(void)
{
	front = NULL;

	while (back)
	{
		free(back->data);
		void* temp = back->next;
		free(back);
		back = temp;
	}
}