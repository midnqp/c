#include "bucket.c"
#include "deps/linenoise/linenoise.h"
#include "utils.c"
#include <ctype.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int THREADMSGPRINT_SHOULDEXIT = 0;
pthread_t THREADMSGWRITE_ID = 0;


void *thread_msg_receive();
void *thread_msg_print();
void *thread_msg_write();

void *thread_msg_receive() {
	pthread_detach(pthread_self());

	for (int i = 0; i < 5; i++) {
		int random = rand_int(1, 3);
		sleep(random);
		add_to_recvbucket();
	}

	return NULL;
}

void *thread_msg_print() {
	pthread_detach(pthread_self());

	while (1) {
		char *recvmsg = get_unflushed_from_recvbucket();
		char *sendmsg = get_unflushed_from_sendbucket();
		bool should_flush = false;
		char *result = malloc(1);
		strcpy(result, "");

		if (strcmp(recvmsg, "") != 0) {
			strappend(&result, "friend_uid: ");
			strappend(&result, recvmsg);
			strappend(&result, "\r\n");
			should_flush = true;
		}

		if (strcmp(sendmsg, "") != 0) {
			strappend(&result, "muhammad: ");
			strappend(&result, sendmsg);
			strappend(&result, "\r\n");
			should_flush = true;
		}

		if (should_flush == true) {
			if (THREADMSGWRITE_ID) {
				pthread_cancel(THREADMSGWRITE_ID);
				THREADMSGWRITE_ID = 0;
			}

			printf("\33[2K\r");
			printf("%s", result);
			fflush(stdout);
			free(result);

			pthread_t T3;
			pthread_create(&T3, NULL, thread_msg_write, NULL);
		} else {
			flusher_timeout_n++;
			free(result);
			sleep(1);
		}

		if (THREADMSGPRINT_SHOULDEXIT == 1)
			break;
	}
	return NULL;
}

void write_msg_cleanup() {
	disableRawMode(STDIN_FILENO);
	THREADMSGWRITE_ID = 0;
}

void *thread_msg_write() {
	THREADMSGWRITE_ID = pthread_self();
	pthread_detach(pthread_self());

	pthread_cleanup_push(write_msg_cleanup, NULL);
	while (1) {
		linenoiseRaw(input_buffer, 1024 * 1024, input_prompt);
		if (strcmp(input_buffer, "exit") == 0) {
			THREADMSGPRINT_SHOULDEXIT = 1;
			break;
		} else {
			add_to_sendbucket();
		}
	}
	pthread_cleanup_pop(1);
	return NULL;
}

int main() {
	pthread_t T1, T2, T3;
	pthread_create(&T1, NULL, &thread_msg_receive, NULL);
	pthread_create(&T2, NULL, &thread_msg_print, NULL);
	pthread_create(&T3, NULL, &thread_msg_write, NULL);

	pthread_exit(NULL);
	return 0;
}
