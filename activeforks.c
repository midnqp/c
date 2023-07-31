#include "activeforks.h"

void active_github_forks(char* uname, char* repo) {
    char* req_url = autofree_malloc(256);
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
        int commits = commits_count(uname, repo);

        log("", "%s (%s)    %d stars    %d commits\n", fn, branch, stars, commits);
        log("", "%d KB    %d open issues\n", size, open_issues);
        log("", "fork created on %s\n", forked_at);
        log("", "last worked on %s\n", pushed_at);
        log("", "\n");
    }

    free(body);
    json_decref(jsondata);
}