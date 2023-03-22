#include "lib.h"

#ifndef FLUSH_BUCKET_H
#define FLUSH_BUCKET_H

int recvmsg_bucket_len = 0;
char *recvmsg_bucket[1024];

int sendmsg_bucket_len = 0;
char *sendmsg_bucket[1024];

//char input_buffer[1024 * 1024] = "a cup of tea";
//int input_buffer_len = 12;
char input_buffer[1024 * 1024] = "";
int input_buffer_len = 0;

const char *prompt = ">>> ";
#define RAND_MAX_SEC 3

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

int flusherrecv_len = 0;
int flusherrecv_list[1024];
#define FLUSHER_TIMEOUT 10
int flusher_timeout_n = 0;
int flushersent_len = 0;
int flushersent_list[1024];

char *get_unflushed_from_recvbucket() {
  for (int i = 0; i < recvmsg_bucket_len; i++) {
    if (arr_has(flusherrecv_list, flusherrecv_len, i) == false) {
      flusherrecv_list[flusherrecv_len++] = i;
      return recvmsg_bucket[i];
    }
  }
  return "";
}

char *get_unflushed_from_sendbucket() {
  for (int idx = 0; idx < sendmsg_bucket_len; idx++) {
    if (arr_has(flushersent_list, flushersent_len, idx) == false) {
      flushersent_list[flushersent_len++] = idx;
      return sendmsg_bucket[idx];
    }
  }
  return "";
}

void add_to_sendbucket() {
  sendmsg_bucket[sendmsg_bucket_len++] = strdup(input_buffer);
  //sendmsg_bucket[sendmsg_bucket_len++] = "hi";

  input_buffer_len = 0;
  input_buffer[input_buffer_len] = '\0';
}

void add_to_inputbuffer(char c) {
  if (isprint(c)) {
    input_buffer[input_buffer_len++] = c;
    input_buffer[input_buffer_len] = '\0';
  }
}

#endif
