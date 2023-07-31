#ifndef _ACTIVEFORKS_H
#define _ACTIVEFORKS_H

#include <stdlib.h>

#ifdef _WIN32
#include "deps/pcre/include/regex.h"
#include <io.h>
#else
#include <regex.h>
#include <unistd.h>
#endif

#include "autofree.h"
#include "commitcount.h"
#include "deps/curl/curl.h"
#include "deps/jansson/jansson.h"
#include "deps/sc/sc_str.h"
#include "lib.h"

void active_github_forks(char *uname, char *repo);

#endif // _ACTIVEFORKS_H