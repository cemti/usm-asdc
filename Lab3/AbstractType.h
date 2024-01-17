#pragma once
#include "database.h"

struct AbstractType
{
	const char* name;
	void (*print)(void*, FILE*);
	void (*insert)(void*, struct Entry*);
	struct Entry* (*find)(void*);
	void (*erase)(void*);
};