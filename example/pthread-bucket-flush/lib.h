#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdarg.h>
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

#ifndef LIB_H
#define LIB_H

static struct termios original_terminal; /* In order to restore at exit.*/

struct config {
  int rawmode;
  int cx, cy;
  int screenrows, screencols;
  /** The initial/reference-point for a prompt. */
  int cx0, cy0;
};

static struct config E = { .cx0=-1, .cy0=-1 };

enum KEY_ACTION {
  KEY_NULL = 0,    /* NULL */
  CTRL_C = 3,      /* Ctrl-c */
  CTRL_D = 4,      /* Ctrl-d */
  CTRL_F = 6,      /* Ctrl-f */
  CTRL_H = 8,      /* Ctrl-h */
  TAB = 9,         /* Tab */
  CTRL_L = 12,     /* Ctrl+l */
  ENTER = 13,      /* Enter */
  CTRL_Q = 17,     /* Ctrl-q */
  CTRL_S = 19,     /* Ctrl-s */
  CTRL_U = 21,     /* Ctrl-u */
  ESC = 27,        /* Escape */
  BACKSPACE = 127, /* Backspace */
  /* The following are just soft codes, not really reported by the
   * terminal directly. */
  ARROW_LEFT = 1000,
  ARROW_RIGHT,
  ARROW_UP,
  ARROW_DOWN,
  DEL_KEY,
  HOME_KEY,
  END_KEY,
  PAGE_UP,
  PAGE_DOWN
};


#define rawmode_printf(...)                                                    \
  do {                                                                         \
    disableRawMode(STDIN_FILENO);                                              \
    printf(__VA_ARGS__);                                                       \
    fflush(stdout);                                                            \
    enableRawMode(STDIN_FILENO);                                               \
  } while (0)


void disableRawMode(int fd) {
  /* Don't even check the return value as it's too late. */
  if (E.rawmode) {
    tcsetattr(fd, TCSAFLUSH, &original_terminal);
    E.rawmode = 0;
  }
}

void editorAtExit(void) { disableRawMode(STDIN_FILENO); }

int enableRawMode(int fd) {
  struct termios raw;

  if (E.rawmode)
    return 0; /* Already enabled. */
  if (!isatty(STDIN_FILENO))
    goto fatal;
  if (tcgetattr(fd, &original_terminal) == -1)
    goto fatal;

  raw = original_terminal; /* modify the original mode */
  /* input modes: no break, no CR to NL, no parity check, no strip char,
   * no start/stop output control. */
  raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  /* output modes - disable post processing */
  raw.c_oflag &= ~(OPOST);
  /* control modes - set 8 bit chars */
  raw.c_cflag |= (CS8);
  /* local modes - choing off, canonical off, no extended functions,
   * no signal chars (^Z,^C) */
  raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  /* control chars - set return condition: min number of bytes and timer. */
  raw.c_cc[VMIN] = 0;  /* Return each byte, or zero for timeout. */
  raw.c_cc[VTIME] = 1; /* 100 ms timeout (unit is tens of second). */

  /* put terminal in raw mode after flushing */
  if (tcsetattr(fd, TCSAFLUSH, &raw) < 0)
    goto fatal;
  E.rawmode = 1;
  return 0;

fatal:
  errno = ENOTTY;
  return -1;
}

/* Read a key from the terminal put in raw mode, trying to handle
 * escape sequences. */
int editorReadKey(int fd) {
    int nread;
    char c, seq[3];
    while ((nread = read(fd,&c,1)) == 0);
    if (nread == -1) exit(1);

    while(1) {
        switch(c) {
        case ESC:    /* escape sequence */
            /* If this is just an ESC, we'll timeout here. */
            if (read(fd,seq,1) == 0) return ESC;
            if (read(fd,seq+1,1) == 0) return ESC;

            /* ESC [ sequences. */
            if (seq[0] == '[') {
                if (seq[1] >= '0' && seq[1] <= '9') {
                    /* Extended escape, read additional byte. */
                    if (read(fd,seq+2,1) == 0) return ESC;
                    if (seq[2] == '~') {
                        switch(seq[1]) {
                        case '3': return DEL_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                        }
                    }
                } else {
                    switch(seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                    }
                }
            }

            /* ESC O sequences. */
            else if (seq[0] == 'O') {
                switch(seq[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
                }
            }
            break;
        default:
            return c;
        }
    }
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

#define setCursorPosition(x,y) do {\
	printf("\x1b[?25l");\
	printf("\x1b[0K");\
	printf("\x1b[%d;%dH", (y), (x));\
	printf("\x1b[?25h");\
} while(0);

int getCursorPosition(int ifd, int ofd, int *rows, int *cols) {
  char buf[32];
  unsigned int i = 0;

  // Report cursor location
  if (write(ofd, "\x1b[6n", 4) != 4)
    return -1;

  // Read the response: ESC [ rows ; cols R
  while (i < sizeof(buf) - 1) {
    if (read(ifd, buf + i, 1) != 1)
      break;
    if (buf[i] == 'R')
      break;
    i++;
  }
  buf[i] = '\0';

  if (buf[0] != ESC || buf[1] != '[')
    return -1;
  if (sscanf(buf + 2, "%d;%d", rows, cols) != 2)
    return -1;
  return 0;
}

int getWindowSize(int ifd, int ofd, int *rows, int *cols) {
  struct winsize ws;

  if (ioctl(1, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
    int orig_row, orig_col, retval;

    retval = getCursorPosition(ifd, ofd, &orig_row, &orig_col);
    if (retval == -1)
      goto failed;

    if (write(ofd, "\x1b[999C\x1b[999B", 12) != 12)
      goto failed;
    retval = getCursorPosition(ifd, ofd, rows, cols);
    if (retval == -1)
      goto failed;

    char seq[32];
    snprintf(seq, 32, "\x1b[%d;%dH", orig_row, orig_col);
    if (write(ofd, seq, strlen(seq)) == -1) {
    }
    return 0;
  } else {
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
  }

failed:
  return -1;
}

#endif
