#include "autofree.h"

#ifndef DEBUG
#define DEBUG 0
#endif

#define debuglog(label, ...) \
	do { \
		if (!DEBUG) break; \
		printf("\033[104m%s\033[0m", label); \
		printf(__VA_ARGS__); \
	} while (0);

void** pointers;
int ptrcount = 0;

int isptreq(void* p1, void* p2) {
	char addr1[16];
	char addr2[16];
	snprintf(addr1, 15, "%p", p1);
	snprintf(addr2, 15, "%p", p2);
	return strcmp(addr1, addr2) == 0;
}

int ptr_idx(void* ptr) {
	for (int i = 0; i < ptrcount; i++) {
		int iseq = isptreq(pointers[i], ptr);
		if (iseq == 1) return i;
	}
	return -1;
}

void ptr_remove(void* ptr) {
	int idx = ptr_idx(ptr);
	if (idx == -1) return;
	for (int i = idx; i < ptrcount - 1; i++)
		pointers[i] = pointers[i + 1];

	ptrcount--;
}
void ptr_add(void* ptr) {
	int idx = ptr_idx(ptr);
	if (idx != -1) return;
	size_t sz = (ptrcount + 1) * sizeof(void*);
	// TODO realloc for 5 pointers each time
	pointers = realloc(pointers, sz);
	pointers[ptrcount] = ptr;
	ptrcount++;

}

void autofree_free_all() {
	for (int i = 0; i < ptrcount; i++) {
		if (pointers[i] == NULL)
			continue;
		debuglog("dealloc", " now address %p\n", pointers[i]);

		free(pointers[i]);
		pointers[i] = NULL;
		debuglog("dealloc", " done address, remaining %d\n",
			ptrcount - (i + 1));
	}

	ptrcount = 0;
	debuglog("dealloc", "about to final...\n");
	if (!!pointers || (pointers != NULL)) {
		debuglog("dealloc (pointers)", " address %p\n", pointers);
		free(pointers);
		pointers = NULL;
	}
}

void* autofree_malloc(size_t len) {
	void* result = malloc(len);
	debuglog("alloc", " %zu bytes at %p, total %d\n", len, result,
		ptrcount + 1);
	ptr_add(result);
	return result;
};

void* autofree_realloc(void* ptr, size_t sz) {
	// not having the line below was a major painful bug!
	int autofree_created = ptr_idx(ptr) != -1;
	// since realloc returns new pointer, remove old one
	if (autofree_created)
		ptr_remove(ptr);
	ptr = realloc(ptr, sz);
	// tracking back any autofree-maintained pointer
	if (autofree_created)
		ptr_add(ptr);
	return ptr;
}

void autofree_free(void* ptr) {
	free(ptr);
	ptr_remove(ptr);
}