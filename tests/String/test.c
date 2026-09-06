#include <string.h>

int isTest(const char *const lhs, const char *const rhs) {
  return (strcmp(lhs, rhs) == 0);
}

int test_String_copys(int id);
int test_String_copyi(int id);

int main(int argc, char **argv) {
  int result = 0;
  if (strcmp(argv[1], "String_copys_1") == 0) {
    result = test_String_copys(1);
  }
  if (strcmp(argv[1], "String_copys_2") == 0) {
    result = test_String_copys(2);
  }
  if (strcmp(argv[1], "String_copyi_1") == 0) {
    result = test_String_copyi(1);
  }
  if (strcmp(argv[1], "String_copyi_2") == 0) {
    result = test_String_copyi(2);
  }
  if (strcmp(argv[1], "String_copyi_3") == 0) {
    result = test_String_copyi(3);
  }
  if (strcmp(argv[1], "String_copyi_4") == 0) {
    result = test_String_copyi(4);
  }
  if (isTest(argv[1], "String_copyi_5")) {
    result = test_String_copyi(5);
  }

  return result;
}
