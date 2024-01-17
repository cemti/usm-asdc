#include "database.h"
#include <string.h>

void write_entry(FILE* fout, struct Entry* entry)
{
	fprintf(fout, "%zu,%s,%s,%s,%c,%f,%hhu.%hhu.%hhu.%hhu,%s\n",
		entry->id, entry->firstName, entry->secondName, entry->email, entry->female ? 'F' : 'M',
		entry->tax, entry->ip[0], entry->ip[1], entry->ip[2], entry->ip[3], entry->country);
}

static struct Entry* db;
static size_t count;

struct Entry* get_db(void)
{
	if (db == NULL)
	{
		FILE* fin;

		if (fopen_s(&fin, "database.csv", "r"))
			return NULL;

		struct Entry dummy;

		while (read_entry(fin, &dummy))
			++count;

		db = malloc(count * sizeof(struct Entry));
		rewind(fin);

		for (size_t i = 0; read_entry(fin, &dummy); ++i)
			memcpy_s(db + i, sizeof(struct Entry), &dummy, sizeof(struct Entry));

		fclose(fin);
	}

	return db;
}

void free_db(void)
{
	free(db);
	db = NULL;
	count = 0;
}

size_t db_count(void)
{
	return count;
}

bool read_entry(FILE* fin, struct Entry* entry)
{
	if (fin == NULL)
		return false;

	size_t id;
	fscanf_s(fin, "%zu,", &id);

	if (feof(fin))
		return false;

	entry->id = id;

	fscanf_s(fin, "%[^,],%[^,],%[^,],", entry->firstName, 50, entry->secondName, 50, entry->email, 100);

	{
		char g;
		fscanf_s(fin, "%c,", &g, 1);
		entry->female = (g | ' ') == 'f';
	}

	fscanf_s(fin, "%lf,%hhu.%hhu.%hhu.%hhu,%s", &entry->tax, &entry->ip[0], &entry->ip[1], &entry->ip[2], &entry->ip[3], entry->country, 3);
	return true;
}

static struct CemNode
{
	void* data;
	struct CemNode* next;
} *head, *tail;

static size_t allocs, memusage;

size_t cem_allocations(void)
{
	return allocs;
}

size_t cem_memusage(void)
{
	return memusage;
}

void* cem_calloc(size_t n, size_t size)
{
	if (head == NULL)
		head = tail = calloc(1, sizeof(struct CemNode));
	else
		tail = tail->next = calloc(1, sizeof(struct CemNode));

	tail->data = calloc(n, size);
	++allocs;
	memusage += n * size;
	return tail->data;
}

void* cem_realloc(void* ptr, size_t size)
{
	for (struct CemNode* it = head; it; it = it->next)
	{
		if (it->data == ptr)
		{
			void* newPtr = realloc(it->data, size);

			if (newPtr == NULL)
				return NULL;

			it->data = newPtr;
			++allocs;
			memusage += size;
			return newPtr;
		}
	}

	return NULL;
}

void cem_collect(void)
{
	while (head)
	{
		void* next = head->next;
		free(head->data);
		free(head);
		head = next;
	}

	tail = NULL;
	memusage = allocs = 0;
}