#include <regex.h>
#include <stdlib.h>
#include <unistd.h>

#include "lib.h"
#include "sc/sc_str.h"

/** Get string value from json object. */
#define jobj_str(object, key) json_string_value(json_object_get(object, key))

#define jobj_num(object, key) json_number_value(json_object_get(object, key))

#define json_check(data, err)                                 \
  {                                                           \
    const char* str = "unexpected token in position";         \
    if (!data) log("jansson", " %s %d\n", str, err.position); \
  }

void get_cat_facts() {
  log("app", " get cat facts by api\n");
  char* _facts = req_get("https://catfact.ninja/facts");
  json_error_t jsonerr;
  json_t* jsondata = json_loads(_facts, 0, &jsonerr);
  json_check(jsondata, jsonerr);
  json_t* factarr = json_object_get(jsondata, "data");

  size_t idx;
  json_t* v;
  json_array_foreach(factarr, idx, v) {
    const char* f = json_string_value(json_object_get(v, "fact"));
    char* fact = NULL;
    if (strlen(f) > 70) {
      fact = sc_str_create_len(f, 70);
      sc_str_append(&fact, "...");
    } else
      fact = sc_str_create(f);
    printf("%8zu  %s\n", idx + 1, fact);
    sc_str_destroy(&fact);
  }

  free(_facts);
  json_decref(jsondata);
  free(_facts);
}

int commit_fmt(const char* text) {
  const char* newline = strchr(text, '\n');
  if (!newline)
    return strlen(text);
  else
    return (int)(newline - text);
}

struct curlstr {
  char* str;
  size_t len;
};

size_t curl_cb(void* p, size_t size, size_t count, struct curlstr* r) {
  size_t total = size * count;
  size_t newlen = r->len + total;
  r->str = realloc(r->str, newlen + 1);
  memcpy(r->str + r->len, p, total);
  r->str[newlen] = '\0';
  r->len = newlen;
  return total;
}

/** Regex to get the number of pages. */
regmatch_t* header_regex(char* textToCheck) {
  regex_t compiledRegex;
  int reti;
  char messageBuffer[100];

  reti = regcomp(&compiledRegex, "page=[0-9]+>; rel=\"last\"",
                 REG_EXTENDED | REG_ICASE);

  /* Execute compiled regular expression */
  static regmatch_t pmatch[2];
  reti = regexec(&compiledRegex, textToCheck, 1, pmatch, 0);
  if (!reti) {
  } else if (reti == REG_NOMATCH) {
  } else {
    regerror(reti, &compiledRegex, messageBuffer, sizeof(messageBuffer));
  }

  /* Free memory allocated to the pattern buffer by regcomp() */
  regfree(&compiledRegex);
  return pmatch;
}

int count_commits(char* user, char* repo) {
  const char* fmt = "https://api.github.com/repos/%s/%s/commits?%s";
  char* url = __alloc(256);
  /*const char* query = "per_page=100";*/
  const char* query = "";
  snprintf(url, 256, fmt, user, repo, query);
  log("debug", " request url is %s\n", url);

  CURL* curl;

  struct curlstr curlres;
  curlres.len = 0;
  curlres.str = malloc(1);
  strcpy(curlres.str, "");

  struct curl_slist* headers = NULL;
  headers = curl_slist_append(
      headers,
      "user-agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, "
      "like Gecko) Chrome/108.0.0.0 Safari/537.36");
  char* ghauthtoken = getenv("GH_AUTH");
  if (ghauthtoken == NULL) {
    log("error", " GH_AUTH env var required\n");
  }
  char authheader[256];
  snprintf(authheader, 256, "authorization: %s", ghauthtoken);
  headers = curl_slist_append(headers, authheader);

  log("debug", " curl headers set\n");
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
  /*log("response body", " %s", curlres.str);*/

  struct curl_header* res_header;
  CURLHcode hcode =
      curl_easy_header(curl, "link", 0, CURLH_HEADER, -1, &res_header);
  log("debug", " found header? %s\n", hcode == CURLHE_OK ? "true" : "false");
  char* _page_count;
  if (hcode == CURLHE_OK) {
    log("debug", " res header: %s\n", res_header->value);
    regmatch_t* regm = header_regex(res_header->value);
    _page_count = sc_str_create(res_header->value);
    sc_str_substring(&_page_count, regm->rm_so + 5, regm->rm_eo - 13);
  }
  int page_count = atoi(_page_count);

  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  free(curlres.str);

  if (hcode == CURLHE_OK) sc_str_destroy(&_page_count);
  return page_count;
}

/** Shows first few commits. */
void show_commits(char* user, char* repo) {
  const char* urlfmt = "https://api.github.com/repos/%s/%s/commits";
  char url[256];
  snprintf(url, 256, urlfmt, user, repo);
  char* response = req_get(url);
  json_error_t jsonerr;
  json_t* data = json_loads(response, 0, &jsonerr);
  free(response);
  json_check(data, jsonerr);
  for (size_t i = 0; i < json_array_size(data); i++) {
    json_t* d = json_array_get(data, i);
    const char* sha = json_string_value(json_object_get(d, "sha"));
    json_t* commit = json_object_get(d, "commit");
    const char* message = json_string_value(json_object_get(commit, "message"));
    printf("%.8s %.*s\n", sha, commit_fmt(message), message);
  }
  json_decref(data);
}

void active_github_forks() {
  log("app", " github active forks\n");
  char* uname = __alloc(256);
  char* repo = __alloc(256);
  /*printf("username: \n");*/
  strcpy(uname, "midnqp");
  /*qio_gets(STDIN_FILENO, uname, 256, 10 * 1000);*/
  /*printf("repo: \n");*/
  strcpy(repo, "cpy");
  /*qio_gets(STDIN_FILENO, repo, 256, 10 * 1000);*/

  char* req_url = __alloc(256);
  const char* query = "sort=stargazers&per_page=100";
  const char* base = "https://api.github.com/repos";
  const char* fmt = "%s/%s/%s/forks?%s";
  snprintf(req_url, 256, fmt, base, uname, repo, query);
  char* body = req_get(req_url);
  /*log("debug", " %s\n", body);*/

  json_error_t jsonerr;
  json_t* jsondata = json_loads(body, 0, &jsonerr);
  json_check(jsondata, jsonerr);
  size_t i;
  json_t* v;
  json_array_foreach(jsondata, i, v) {
    const char* fn = jobj_str(v, "full_name");
    const char* branch = jobj_str(v, "default_branch");
    int stars = jobj_num(v, "stargazers_count");
    int size = jobj_num(v, "size");
    const char* pushed_at = jobj_str(v, "pushed_at");
    const char* forked_at = jobj_str(v, "created_at");
    int open_issues = jobj_num(v, "open_issues");
    int commits = count_commits(uname, repo);

    log("", "%s (%s)    %d stars    %d commits\n", fn, branch, stars, commits);
    log("", "%d KB    %d open issues\n", size, open_issues);
    log("", "fork created on %s\n", forked_at);
    log("", "last worked on %s\n", pushed_at);
    log("", "\n");
  }

  count_commits(uname, repo);
  free(body);
  json_decref(jsondata);
}

int main() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  active_github_forks();
  dealloc();
  curl_global_cleanup();
}
