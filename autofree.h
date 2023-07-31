#ifndef _AUTOFREE_H
#define _AUTOFREE_H

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

void autofree_free(void* ptr);
void autofree_free_all();
void* autofree_malloc(size_t len);
void* autofree_realloc(void* ptr, size_t sz);

#endif // _AUTOFREE_H