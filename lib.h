#ifndef _LIB_H
#define _LIB_H
#include <malloc.h>
#include <stdio.h>
#include <string.h>

#include "curl/curl.h"
#include "deps/jansson/jansson.h"
#include "sc/sc_str.h"

#define _add(a, b) a b

#if defined(NDEBUG) || defined(DEBUG) || defined(debug)
#define DEBUG_MODE 1
#else
#define DEBUG_MODE 0
#endif

#define log(label, ...)                                          \
  {                                                              \
    const char* l = "";                                          \
    if (strcmp(label, "") == 1) l = _add(_add("[", label), "]"); \
    if (strcmp(label, "debug") == 0) {                           \
      if (DEBUG_MODE == 1 || strcmp(getenv("DEBUG"), "") != 0) {                                          \
        printf("%s", l);                                         \
        printf(__VA_ARGS__);                                     \
      }                                                          \
    } else {                                                     \
      printf("%s", l);                                           \
      printf(__VA_ARGS__);                                       \
    }                                                            \
  }

struct curlstr {
    char* str;
    size_t len;
};

void curl_init_once();
void curl_exit_once();
size_t curl_cb(void* p, size_t size, size_t count, struct curlstr* r);
char* req_get(char* url);

/** Get string value from json object. */
#define jobj_str(object, key) json_string_value(json_object_get(object, key))

#define jobj_num(object, key) json_number_value(json_object_get(object, key))

#define json_check(data, err)                                                  \
  {                                                                            \
    const char *str = "unexpected token in position";                          \
    if (!data)                                                                 \
      log("jansson", " %s %d\n", str, err.position);                           \
  }

#endif // _LIB_H