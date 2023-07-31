#include "lib.h"
#include <stddef.h>
#include "curl/curl.h"
#include "curl/easy.h"

static int _curl_state = 0; // 1 means inited

void curl_init_once() {
    if (_curl_state != 0) return;
    _curl_state = 1;
    curl_global_init(CURL_GLOBAL_DEFAULT);
  
}

void curl_exit_once() {
    if (_curl_state != 1) return;
    _curl_state = 0;
    curl_global_cleanup();
}

size_t curl_cb(void* p, size_t size, size_t count, struct curlstr* r) {
  size_t total = size * count;
  size_t newlen = r->len + total;
  r->str = realloc(r->str, newlen + 1);
  memcpy(r->str + r->len, p, total);
  r->str[newlen] = '\0';
  r->len = newlen;
  return total;
}

char* req_get(char* url) {
  curl_init_once();
  CURL* curl;
  CURLcode code;

  struct curlstr curlres;
  curlres.len = 0;
  curlres.str = malloc(1);
  strcpy(curlres.str, "");

  struct curl_slist* headers = NULL;
  headers = curl_slist_append(
      headers,
      "user-agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, "
      "like Gecko) Chrome/108.0.0.0 Safari/537.36");

  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlres);

  code = curl_easy_perform(curl);
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);

  return curlres.str;
}