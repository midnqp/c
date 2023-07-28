#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

static void **pointers;
static int ptrcount = 0;

void deallocate_all() {
  for (int i = 0; i < ptrcount; i++) {
    if (pointers[i] == NULL)
      continue;
    free(pointers[i]);
    pointers[i] = NULL;
    ptrcount--;
  }
  free(pointers);
}

void *alloc(size_t len) {
  void *result = malloc(len);
  // realloc and resize the freeable_list
  pointers = realloc(pointers, (ptrcount + 1) * sizeof(void *));
  pointers[ptrcount] = result;
  ptrcount++;
  return result;
};

int main() {
  char* name = alloc(9);
  strcpy(name, "muhammad");
  char* greeting = alloc(6);
  strcpy(greeting, "hello");
  printf("%s %s\n", greeting, name);

  deallocate_all();
}
