#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "./lib.h"

int recvmsg_bucket_len = 0;
char *recvmsg_bucket[1024];

int sendmsg_bucket_len = 0;
char *sendmsg_bucket[1024];

char input_buffer[1024 * 1024] = {'\0'};
int input_buffer_len = 1;

char *prompt = "muhammad >>> ";
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

  /*printf("(added to bucket)\n");*/
  /*printf("(new message received)\n");*/
}

int rand_int(int min, int max) {
  int r = rand() % (max + 1 - min) + min;
  return r;
}

void *receive_msg_thread(void *args) {
  /*pthread_detach(pthread_self());*/
  for (int i = 0; i < 5; i++) {
    int random = rand_int(1, RAND_MAX_SEC);
    sleep(random);
    add_to_recvbucket();
  }
  /*pthread_exit(NULL);*/
}

int flusherrecv_len = 0;
int flusherrecv_list[1024];
#define FLUSHER_TIMEOUT 10
int flusher_timeout_n = 0;

bool arr_has(int arr[], int arrlen, int value) {
  for (int i = 0; i < arrlen; i++) {
    if (arr[i] == value)
      return true;
  }
  return false;
}

char *get_unflushed_from_recvbucket() {
  for (int i = 0; i < recvmsg_bucket_len; i++) {
    if (arr_has(flusherrecv_list, flusherrecv_len, i) == false) {
      flusherrecv_list[flusherrecv_len++] = i;
      return recvmsg_bucket[i];
    }
  }
  return "";
}

int flushersent_len = 0;
int flushersent_list[1024];
char *get_unflushed_from_sendbucket() {
  for (int idx = 0; idx < sendmsg_bucket_len; idx++) {
    if (arr_has(flushersent_list, flushersent_len, idx) == false) {
      flushersent_list[flushersent_len++] = idx;
      return sendmsg_bucket[idx];
    }
  }
  return "";
}

void realloc_str(char **ptr, char *src) {
  int ptr_len = strlen(*ptr);
  int src_len = strlen(src);
  if (src_len > 0) {
    int total = ptr_len + src_len;
    if (src[src_len - 1] != '\0')
      total++;
    else if (*ptr[ptr_len - 1] != '\0')
      total++;
    *ptr = realloc(*ptr, total);
  }
}

void strappend(char **ptr, char *src) {
  realloc_str(ptr, src);
  strcat(*ptr, src);
}

void *print_msg_thread(void *args) {
  /*pthread_detach(pthread_self());*/

  while (1) {
    char *recvmsg = get_unflushed_from_recvbucket();
    char *sendmsg = get_unflushed_from_sendbucket();
    bool flushing = false;
    char *result = malloc(1);
    strcpy(result, "");

    if (strcmp(recvmsg, "") != 0) {
      /*printf("new message: %s\n", recvmsg);*/
      strappend(&result, "friend_uid: ");
      strappend(&result, recvmsg);
      strappend(&result, "\n");
      flusher_timeout_n = 0;
      flushing = true;
    }

    if (strcmp(sendmsg, "") != 0) {
      strappend(&result, "muhammad: ");
      strappend(&result, sendmsg);
      strappend(&result, "\n");
      flusher_timeout_n = 0;
      flushing = true;
    }

    if (flushing == true) {
		// TODO
      /*strappend(&result, prompt);*/
      /*strappend(&result, input_buffer);*/
	  disableRawMode(STDIN_FILENO);
      printf("\33[2K\r");
      printf("%s", result);
      fflush(stdout);
	  enableRawMode(STDIN_FILENO);
      free(result);
    } else {
      /*printf("system: timing out...%d\n", flusher_timeout_n);*/
      free(result); // creating/freeing a buffer on each tick
      /*if (flusher_timeout_n == FLUSHER_TIMEOUT)*/
        /*break;*/
      flusher_timeout_n++;
      sleep(1);
    }
  }
}

void add_to_sendbucket() {
  sendmsg_bucket[sendmsg_bucket_len++] = input_buffer;
  input_buffer_len = 0;
  input_buffer[input_buffer_len] = '\0';
}

void add_to_inputbuffer(char c) {
  /*if (isprint(c)) {*/
    input_buffer[input_buffer_len++] = c;
    input_buffer[input_buffer_len] = '\0';
  /*}*/
}

int main() {
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, receive_msg_thread, NULL);
  pthread_create(&thread2, NULL, print_msg_thread, NULL);

  int n_pressed_enter = 0;
  printf("%s", prompt);
  fflush(stdout);
  enableRawMode(STDIN_FILENO);
  while (1) {
    char c = editorReadKey(STDIN_FILENO);
    add_to_inputbuffer(c);

    switch (c) {
    case ENTER: // send!
      add_to_sendbucket();
      printf("\33[2K\r");
  		printf("%s", prompt);
		fflush(stdout);
      break;

    }
  }

  /*pthread_join(thread1, NULL);*/
  /*pthread_join(thread2, NULL);*/
  pthread_exit(NULL);

  return 0;
}
