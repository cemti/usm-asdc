#include "database.h"
#include <string.h>

int compare_entries(struct Entry* lhs, struct Entry* rhs, size_t col)
{
	switch (col)
	{
	default:
		return lhs->id > rhs->id ? 1 : lhs->id == rhs->id ? 0 : -1;

	case 1:
		return strcmp(lhs->firstName, rhs->firstName);

	case 2:
		return strcmp(lhs->secondName, rhs->secondName);

	case 3:
		return strcmp(lhs->email, rhs->email);

	case 4:
		return lhs->female > rhs->female ? 1 : lhs->female == rhs->female ? 0 : -1;

	case 5:
		return lhs->tax > rhs->tax ? 1 : lhs->tax == rhs->tax ? 0 : -1;

	case 6:
		return memcmp(lhs->ip, rhs->ip, sizeof lhs->ip);

	case 7:
		return strcmp(lhs->country, rhs->country);
	}
}

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