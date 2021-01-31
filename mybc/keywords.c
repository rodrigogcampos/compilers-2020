#include <string.h>
#include <keywords.h>

char *keyword[] = {
  "quit",
  "exit",
};

int iskeyword(const char *name) {
  int i;

  for (i = QUIT; i < EXIT + 1; i++) {
    if (strcmp(keyword[i - QUIT], name) == 0) return i;
  }

  return 0;
}
