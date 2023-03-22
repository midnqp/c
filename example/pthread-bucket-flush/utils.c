#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#ifndef _UTILS_H
#define _UTILS_H

/* auto reallocs target ptr based on src string! */
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

bool arr_has(int arr[], int arrlen, int value) {
  for (int i = 0; i < arrlen; i++) {
    if (arr[i] == value)
      return true;
  }
  return false;
}

int rand_int(int min, int max) {
  int r = rand() % (max + 1 - min) + min;
  return r;
}

#endif
