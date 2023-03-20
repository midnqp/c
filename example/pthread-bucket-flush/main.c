#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "bucket-flush.h"
#include "lib.h"

#define rawmode_printf(...)                                                    \
  do {                                                                         \
    disableRawMode(STDIN_FILENO);                                              \
    printf(__VA_ARGS__);                                                       \
    fflush(stdout);                                                            \
    enableRawMode(STDIN_FILENO);                                               \
  } while (0)

pthread_t write_msg_thread_id = 0;
void *write_msg_thread(void *args);

void *receive_msg_thread(void *args) {
  pthread_detach(pthread_self());

  for (int i = 0; i < 5; i++) {
    int random = rand_int(1, RAND_MAX_SEC);
    sleep(random);
    add_to_recvbucket();
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

    if (flushing == true && write_msg_thread_id) {
      disableRawMode(STDIN_FILENO);
      pthread_cancel(write_msg_thread_id);

      printf("\33[2K\r");
      printf("%s", result);
      fflush(stdout);
      enableRawMode(STDIN_FILENO);
      free(result);

      pthread_t T3;
      pthread_create(&T3, NULL, write_msg_thread, NULL);
    } else {
      free(result);
      flusher_timeout_n++;
      sleep(1);
    }
  }
}

void disable_raw_mode() {
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_terminal);
}

void enable_raw_mode() {
  tcgetattr(STDIN_FILENO, &original_terminal);
  atexit(disable_raw_mode);
  struct termios raw = original_terminal;
  raw.c_lflag &= ~(ECHO | ICANON);
  /*raw.c_oflag &= ~(OPOST);*/
  tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void *write_msg_thread(void *args) {
  enableRawMode(STDIN_FILENO);
  while (1) {
    char c = editorReadKey(STDIN_FILENO);
    if (isprint(c)) {
      add_to_inputbuffer(c);
      rawmode_printf("%c", c);
    }
    if (c == ESC) {
      printf("pressed esc\n");
      fflush(stdout);
      disableRawMode(STDIN_FILENO);
      break;
    }
    if (c == '\n' || c == ENTER) {
      input_buffer_len = 0;
      input_buffer[0] = '\0';
      rawmode_printf("\r\n%s",prompt);
      continue;
    }
  }
}

int main() {
  pthread_t T1, T2, T3;
  /*pthread_create(&T1, NULL, receive_msg_thread, NULL);*/
  /*pthread_create(&T2, NULL, print_msg_thread, NULL);*/
  /*pthread_create(&T3, NULL, write_msg_thread, NULL);*/

  int n_pressed_enter = 0;
  printf("%s", prompt);
  fflush(stdout);
  write_msg_thread(NULL);

  /*pthread_exit(NULL);*/
  return 0;
}
