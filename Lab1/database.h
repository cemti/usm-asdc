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

struct Node
{
	struct Entry* entry;
	struct Node *prev, *next;
};

void write_entry(FILE* fout, struct Entry* entry);
void remove_node(struct Node* node);

struct Node* read_db(FILE* fin, size_t* count);
void write_db(FILE* fout, struct Node* head);
void free_db(struct Node* head);

void shuffle_db(struct Node* head);

void* cem_calloc(size_t count, size_t size);
void cem_collect(void);