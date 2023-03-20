
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int main() {
  char *ptr = malloc(1);
  strcpy(ptr, "");

  strappend(&ptr, ""); // this line used to cause error, now fixed.
  strappend(&ptr, "i can add string of any length?!\n");
  strappend(&ptr, "you can assure me that you're not joking?\n");
  strappend(&ptr, "\n");
  strappend(&ptr, "in that case...\n");
  strappend(&ptr, "               thank you\n");
  printf("%s", ptr);
  free(ptr);
}
