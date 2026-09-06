#include "Token.h"
#include <stddef.h>
#include <stdint.h>

int main(int argc, char **argv) {
  Token_Stream stream = {0};
  Token_Stream_init(&stream, 1, &g_defaultAllocator);

  char hw[] = "hello world!";

  printf("%-25s %p\n", "Token_Stream::ptr: ", stream.ptr);
  printf("%-25s %p\n", "Token_Stream::length: ", stream.length);
  printf("%-25s %p\n", "Token_Stream::type: ", stream.type);

  stream.ptr[0] = hw;
  stream.length[0] = ARRAY_SIZE(hw);
  stream.type[0] = TOKEN_TYPE_IDENTIFIER;

  printf("+----| %-15.*s -> %s\n", (int)stream.length[0], stream.ptr[0],
         Token_getType(stream.type[0]));

  Token_Stream_release(&stream, &g_defaultAllocator);
  return 0;
}
