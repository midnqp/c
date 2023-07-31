#include "commitcount.h"

int commits_fmt(const char *text) {
  const char *newline = strchr(text, '\n');
  if (!newline)
    return strlen(text);
  else
    return (int)(newline - text);
}

// Regex to get the number of pages.
regmatch_t *header_regex(char *textToCheck) {
  regex_t compiledRegex;
  int reti;
  char messageBuffer[100];

  reti = regcomp(&compiledRegex, "page=[0-9]+>; rel=\"last\"",
                 REG_EXTENDED | REG_ICASE);

  // Execute compiled regular expression 
  static regmatch_t pmatch[2];
  reti = regexec(&compiledRegex, textToCheck, 1, pmatch, 0);
  if (!reti) {
  } else if (reti == REG_NOMATCH) {
  } else {
    regerror(reti, &compiledRegex, messageBuffer, sizeof(messageBuffer));
  }

  regfree(&compiledRegex);
  return pmatch;
}

int commits_count(const char *user, const char *repo) {
  char *url = autofree_malloc(256);
  const char *fmt = "https://api.github.com/repos/%s/%s/commits?%s";
  //const char *query = "per_page=100";
  const char *query = "";
  int len = snprintf(url, 256, fmt, user, repo, query);
  url[len] = '\0';
  log("debug", " request url is %s\n", url);

  struct curlstr curlres = {.len = 0, .str = malloc(1)};
  strcpy(curlres.str, "");

  struct curl_slist *headers = NULL;
  headers = curl_slist_append(
      headers,
      "user-agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, "
      "like Gecko) Chrome/108.0.0.0 Safari/537.36");
  char authheader[256];
  const char *ghauthtoken = getenv("GH_AUTH");
  if (ghauthtoken == NULL) {
    log("error", " GH_AUTH env var required\n");
  } else
    snprintf(authheader, 256, "authorization: %s", ghauthtoken);
  headers = curl_slist_append(headers, authheader);

  log("debug", " curl headers set\n");
  CURL *curl;
  curl = curl_easy_init();
  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_HTTPGET, 1);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);
  curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_cb);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlres);
  curl_easy_perform(curl);
  log("debug", " curl executed\n");
  log("debug", " response body:\n%s", curlres.str);

  int page_count = 0;
  char *_page_count = sc_str_create("0");
  struct curl_header *res_header;
  CURLHcode hcode;
  hcode = curl_easy_header(curl, "link", 0, CURLH_HEADER, -1, &res_header);
  log("debug", " found header? %s\n", hcode == CURLHE_OK ? "true" : "false");
  if (hcode == CURLHE_OK) {
    log("debug", " res header: %s\n", res_header->value);
    regmatch_t *regm = header_regex(res_header->value);
    sc_str_set(&_page_count, res_header->value);
    sc_str_substring(&_page_count, regm->rm_so + 5, regm->rm_eo - 13);
  }
  page_count = atoi(_page_count);

  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  free(curlres.str);

  sc_str_destroy(&_page_count);
  return page_count;
}

/** Shows first few commits. */
void commits_show(char *user, char *repo) {
  const char *urlfmt = "https://api.github.com/repos/%s/%s/commits";
  char *url = autofree_malloc(256);
  snprintf(url, 256, urlfmt, user, repo);
  char *response = req_get(url);
  json_error_t jsonerr;
  json_t *data = json_loads(response, 0, &jsonerr);
  free(response);
  json_check(data, jsonerr);
  for (size_t i = 0; i < json_array_size(data); i++) {
    json_t *d = json_array_get(data, i);
    const char *sha = json_string_value(json_object_get(d, "sha"));
    json_t *commit = json_object_get(d, "commit");
    const char *message = json_string_value(json_object_get(commit, "message"));
    printf("%.8s %.*s\n", sha, commits_fmt(message), message);
  }
  json_decref(data);
}