#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "deps/linenoise/linenoise.h"

#include "bucket-flush.h"
#include "lib.h"

int FLAG_PRINTMSGTHREAD = 0;
char *buffer = input_buffer;
pthread_t write_msg_thread_id = 0;
void *write_msg_thread(void *args);

void *receive_msg_thread(void *args) {
  pthread_detach(pthread_self());

  for (int i = 0; i < 5; i++) {
    int random = rand_int(1, RAND_MAX_SEC);
    sleep(random);
    add_to_recvbucket();
    /*add_to_sendbucket();*/
  }
}

void *print_msg_thread(void *args) {
  pthread_detach(pthread_self());

  while (1) {
	char *recvmsg = get_unflushed_from_recvbucket();
	char *sendmsg = get_unflushed_from_sendbucket();
	bool flushing = false;
    char *result = malloc(1);
    strcpy(result, "");

    /*printf("\r\nmeanwhile input_buffer is: %s\r\n",input_buffer);*/

    if (strcmp(recvmsg, "") != 0) {
      /*printf("new message: %s\n", recvmsg);*/
      strappend(&result, "friend_uid: ");
      strappend(&result, recvmsg);
      strappend(&result, "\r\n");
      flusher_timeout_n = 0;
      flushing = true;
    }

    if (strcmp(sendmsg, "") != 0) {
      strappend(&result, "muhammad: ");
      strappend(&result, sendmsg);
      strappend(&result, "\r\n");
      flusher_timeout_n = 0;
      flushing = true;
    }

    if (flushing == true) {
      if (write_msg_thread_id) {

      /*printf("(before write_msg kill) existing in input_buffer? %s\r\n", input_buffer);*/
      /*fflush(stdout);*/
        pthread_cancel(write_msg_thread_id);
		write_msg_thread_id = 0;
	 }

      printf("\33[2K\r");
      printf("%s", result);
      fflush(stdout);
      free(result);

      pthread_t T3;
      pthread_create(&T3, NULL, write_msg_thread, NULL);
      /*printf("(after write_msg kill) existing in input_buffer? %s\r\n", input_buffer);*/
      /*fflush(stdout);*/
    } else {
      flusher_timeout_n++;
      free(result);
      /*if (flusher_timeout_n == 100) {*/
      /*printf("print_msg_thread exit\r\n");*/
      /*printf("killing write_msg_thread, text: %s\r\n", buffer);*/
      /*disableRawMode(STDIN_FILENO);*/
      /*pthread_cancel(write_msg_thread_id);*/
      /*disableRawMode(STDIN_FILENO);*/
      /*break;*/
      /*}*/
      sleep(1);
    }

      if (FLAG_PRINTMSGTHREAD == 1)
        break;
  }
}

void* write_msg_cleanup(void* args) {
	disableRawMode(STDIN_FILENO);
	write_msg_thread_id = 0;
}

void *write_msg_thread(void *args) {
  write_msg_thread_id = pthread_self();
  pthread_detach(pthread_self());

  pthread_cleanup_push(write_msg_cleanup, NULL);
  while (1) {
	linenoiseRaw(input_buffer, 1024 * 1024, prompt);
    if (strcmp(input_buffer, "exit") == 0) {
      FLAG_PRINTMSGTHREAD = 1;
      break;
    } else{
	  add_to_sendbucket();
	}
  }
  pthread_cleanup_pop(1);
  printf("write_msg exit\r\n");
  return NULL;
}

int main() {
  pthread_t T1, T2, T3;
  pthread_create(&T1, NULL, receive_msg_thread, NULL);
  pthread_create(&T2, NULL, print_msg_thread, NULL);
  pthread_create(&T3, NULL, write_msg_thread, NULL);

  pthread_exit(NULL);
  return 0;
}
