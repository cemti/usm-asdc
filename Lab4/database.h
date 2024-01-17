#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

struct Entry
{
	size_t id;
	char firstName[50], secondName[50], email[100];
	bool female;
	double tax;
	char ip[4], country[3];
};

struct Entry* get_db(void);
void free_db(void);
size_t db_count(void);

void write_entry(FILE*, struct Entry*);
bool read_entry(FILE*, struct Entry*);

size_t cem_allocations(void);
size_t cem_memusage(void);
void* cem_calloc(size_t, size_t);
void* cem_realloc(void*, size_t);
void cem_collect(void);