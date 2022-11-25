#include <malloc.h>

#define ADD(a, b) a b
#define log(label, ...) \
  printf(ADD(ADD("\033[104m", label), "\033[0m") __VA_ARGS__)

static void** freeable;
static int freeable_count = 0;

void dealloc() {
  for (int i = 0; i < freeable_count; i++) {
    if (freeable[i] == NULL) continue;
    log("dealloc", " address %p\n", freeable[i]);
    free(freeable[i]);
    freeable[i] = NULL;
  }
}

void* alloc(int len) {
  void* result = malloc(len);
  log("alloc", " %d bytes\n", len);
  freeable[freeable_count] = result;
  freeable_count++;
  return result;
};

#include <stdio.h>
#include <string.h>

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

int main() {
  char* a = alloc(9);
  strcpy(a, "muhammad");
  char* b = alloc(6);
  strcpy(b, "hello");
  printf("%s %s\n", b, a);

  dealloc();
}
