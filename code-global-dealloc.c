#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>

static void **freeable_list;
static int freeable_count = 0;

void dealloc() {
  for (int i = 0; i < freeable_count; i++) {
    if (freeable_list[i] == NULL)
      continue;
    free(freeable_list[i]);
    freeable_list[i] = NULL;
    freeable_count--;
  }
  free(freeable_list);
}

void *alloc(size_t len) {
  void *result = malloc(len);
  // realloc and resize the freeable_list
  freeable_list = realloc(freeable_list, (freeable_count + 1) * sizeof(void *));
  freeable_list[freeable_count] = result;
  freeable_count++;
  return result;
};

/*
#ifdef free
#undef free
#define free(x) \
  {             \
    free(x);    \
    x = NULL;   \
  }
#endif

#ifdef malloc
#undef malloc
#define malloc(x) alloc(x)
#endif
*/

int main() {
  char* a = alloc(9);
  strcpy(a, "muhammad");
  char* b = alloc(6);
  strcpy(b, "hello");
  printf("%s %s\n", b, a);

  dealloc();
}
