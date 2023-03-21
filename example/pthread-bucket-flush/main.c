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

void refresh_prompt() {
  char *buf = malloc(1);
  strcpy(buf, "");
  strappend(&buf, prompt);
  strappend(&buf, input_buffer);
  write(STDOUT_FILENO, buf, strlen(buf));
  fflush(stdout);
  free(buf);

  if (E.cx0 == -1 || E.cx == -1 || E.cy0 == -1 || E.cy == -1) {
    getWindowSize(STDIN_FILENO, STDOUT_FILENO, &E.screenrows, &E.screencols);
    getCursorPosition(STDIN_FILENO, STDOUT_FILENO, &E.cx, &E.cy);
    E.cx0 = E.cx;
    E.cy0 = E.cy;
  }
  setCursorPosition(E.cy+1, E.cx);
}

void *write_msg_thread(void *args) {
  enableRawMode(STDIN_FILENO);
  refresh_prompt();

  while (1) {
    int c = editorReadKey(STDIN_FILENO);
    add_to_inputbuffer(c);

    if (c == ESC) {
      printf("\r\nBye!\r\n");
      fflush(stdout);
      break;
    }

    else if (c == ENTER) {
      printf("\r\nThe message text is \"%s\"", input_buffer);
      input_buffer_len = 0;
      input_buffer[0] = '\0';
      E.cx++;
      E.cx0++;
      printf("\r\n");
      fflush(stdout);
    }

    else if (c == ARROW_LEFT) {
      if (E.cy > E.cy0) {
        /*printf("%d %d\r\n", E.cy, E.cy0);*/
        /*fflush(stdout);*/
        E.cy--;
      }
    }

    else if (c == ARROW_UP && input_buffer[0] == '\0') {
      /*E.cx++;*/
      /*E.line_cx++;*/
      /*input_buffer = history[history_len];*/
    }

    else if (c == BACKSPACE) {
      if (E.cy > E.cy0) {
        E.cy--;
--input_buffer_len;
        input_buffer[input_buffer_len] = '\0';
      }
    } else {
      E.cy++;
	}

    refresh_prompt();
  }
}

int main() {
  pthread_t T1, T2, T3;
  /*pthread_create(&T1, NULL, receive_msg_thread, NULL);*/
  /*pthread_create(&T2, NULL, print_msg_thread, NULL);*/
  /*pthread_create(&T3, NULL, write_msg_thread, NULL);*/

  int n_pressed_enter = 0;
  write_msg_thread(NULL);

  disableRawMode(STDIN_FILENO);
  pthread_exit(NULL);
  return 0;
}
