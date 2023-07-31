#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#include "deps/vld/vld.h"
#endif

#include "activeforks.h"
#include "autofree.h"
#include "catfacts.h"
#include "commitcount.h"
#include "lib.h"

int main() {
  char *name = autofree_malloc(9);
  strcpy(name, "World");
  log("app", "Hello, %s!\n", name);
  autofree_free(name);

  int total = commits_count("midnqp", "cpy");
  log("app", "This repository has %d commits.\n", total);
  log("", "\n\n");

  log("app", "These are the most popular forks of the Linux kernel:\n");
  active_github_forks("torvalds", "linux");
  log("", "\n\n");

  log("app", "By the way, here's some interesting facts about cats!\n");
  get_cat_facts();
  log("", "\n\n");

  autofree_free_all();
  return 0;
}