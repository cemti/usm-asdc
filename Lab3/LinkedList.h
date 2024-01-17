#pragma once
#include "AbstractType.h"

struct Node
{
	struct Entry* entry;
	struct Node* ptr[];
};

struct Node* NewMonoNode(void);
void* InitLinkedList(struct AbstractType*);
void* InitCircularLinkedList(struct AbstractType*);