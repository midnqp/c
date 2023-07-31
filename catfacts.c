#include "catfacts.h"

void get_cat_facts() {
  log("app", " get cat facts by api\n");
  char *_facts = req_get("https://catfact.ninja/facts");
  json_error_t jsonerr;
  json_t *jsondata = json_loads(_facts, 0, &jsonerr);
  json_check(jsondata, jsonerr);
  json_t *factarr = json_object_get(jsondata, "data");

  size_t idx;
  json_t *v;
  json_array_foreach(factarr, idx, v) {
    const char *f = json_string_value(json_object_get(v, "fact"));
    char *fact = NULL;
    if (strlen(f) > 70) {
      fact = sc_str_create_len(f, 70);
      sc_str_append(&fact, "...");
    } else
      fact = sc_str_create(f);
    printf("%8zu  %s\n", idx + 1, fact);
    sc_str_destroy(&fact);
  }

  json_decref(jsondata);
  free(_facts);
}