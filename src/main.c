#include "Lexer.h"
#include <stdio.h>

int main(void) {
  Result result = {0};
  Lexer lexer;
  Token_Stream stream;

  result = Token_Stream_init(&stream, 1024, getDefaultAllocator());
  if (!Result_isSuccess(result)) {
    Result_printData(result);
  }
  result = Lexer_init(&lexer, CMAKE_SOURCE_DIR "/assets/main.gr",
                      getDefaultAllocator());
  if (!Result_isSuccess(result)) {
    Result_printData(result);
  }

  for (size_t i = 0; i < stream.capacity; i += 1) {
    Token_Proxy proxy =
        Token_createProxy(&stream.ptr[i], &stream.length[i], &stream.row[i],
                          &stream.column[i], &stream.type[i]);
    result = Lexer_tokenize(&lexer, &proxy);
    /// [%Row:%Column]: %.*Token -> %TokenType
    printf("[%zu:%zu]: %.*s -> %s\n", stream.row[i], stream.column[i],
           (int)stream.length[i], stream.ptr[i], Token_getType(stream.type[i]));
    stream.count += 1;
  }

  Lexer_release(&lexer, getDefaultAllocator());
  Token_Stream_release(&stream, getDefaultAllocator());
  return 0;
}
