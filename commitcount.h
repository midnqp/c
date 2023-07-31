#ifndef _COMMITCOUNT_H
#define _COMMITCOUNT_H

#ifdef _WIN32
#include "deps/pcre/include/regex.h"
#include <io.h>
#else
#include <regex.h>
#include <unistd.h>
#endif
#include "deps/curl/curl.h"
#include "deps/jansson/jansson.h"
#include "deps/sc/sc_str.h"
#include "lib.h"
#include "autofree.h"

int commits_count(const char *user, const char *repo);

void commits_show(char *user, char *repo);

int commits_fmt(const char *text);

#endif // _COMMITCOUNT_H