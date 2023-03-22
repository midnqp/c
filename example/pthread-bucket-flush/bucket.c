#ifndef _BUCKET_H
#define _BUCKET_H

#include "utils.c"
#include <string.h>
#include <time.h>

static int recvmsg_bucket_len = 0;
static char *recvmsg_bucket[1024];
static int sendmsg_bucket_len = 0;
static char *sendmsg_bucket[1024];
static char input_buffer[1024 * 1024] = "";
const char *input_prompt = ">>> ";
static int flusherrecv_len = 0;
static int flusherrecv_list[1024];
static int flusher_timeout_n = 0;
static int flushersent_len = 0;
static int flushersent_list[1024];

void add_to_recvbucket() {
	time_t raw;
	time(&raw);
	struct tm *timeinfo = localtime(&raw);
	char *timestr = asctime(timeinfo);

	// timestr has a newline at the end
	size_t len = strlen(timestr);
	len--;
	timestr[len] = '\0';
	recvmsg_bucket[recvmsg_bucket_len++] = timestr;
}

void add_to_sendbucket() {
	sendmsg_bucket[sendmsg_bucket_len++] = strdup(input_buffer);
	input_buffer[0] = '\0';
}

char *get_unflushed_from_recvbucket() {
	for (int i = 0; i < recvmsg_bucket_len; i++) {
		if (!arr_has(flusherrecv_list, flusherrecv_len, i)) {
			flusherrecv_list[flusherrecv_len++] = i;
			return recvmsg_bucket[i];
		}
	}
	return "";
}

char *get_unflushed_from_sendbucket() {
	for (int idx = 0; idx < sendmsg_bucket_len; idx++) {
		if (!arr_has(flushersent_list, flushersent_len, idx)) {
			flushersent_list[flushersent_len++] = idx;
			return sendmsg_bucket[idx];
		}
	}
	return "";
}

#endif
