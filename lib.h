#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "curl/curl.h"
#include "deps/jansson/jansson.h"
#include "sc/sc_str.h"

#define _add(a, b) a b

#if defined(NDEBUG) || defined(DEBUG) || defined(debug)
#define debug 1
#else
#define debug 0
#endif

#define log(label, ...)                                          \
  {                                                              \
    const char* l = "";                                          \
    if (strcmp(label, "") == 1) l = _add(_add("[", label), "]"); \
    if (strcmp(label, "debug") == 0) {                           \
      if (debug == 1) {                                          \
        printf("%s", l);                                         \
        printf(__VA_ARGS__);                                     \
      }                                                          \
    } else {                                                     \
      printf("%s", l);                                           \
      printf(__VA_ARGS__);                                       \
    }                                                            \
  }

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
#define malloc(x) __alloc(x)
#endif

void dealloc();
void* __alloc(int len);

char* req_post();
char* req_get(char* url);
void curl_exit_once();
size_t curl_cb(void* p, size_t size, size_t count, struct curlstr* r);