#include "AST/Visitor.h"
#include "Arena.h"
#include "Lexer.h"
#include "Parser.h"
#include <stdio.h>

int main(void) {
  Result result = {0};
  Lexer lexer;
  Token_Stream stream;
  Arena arena;
  Allocator arenaAllocator;

  result = Arena_init(&arena, 1024 * 1024, getDefaultAllocator());
  if (!Result_isSuccess(result)) {
    Result_printData(result);
    goto onError;
  }
  arenaAllocator = Arena_getAllocator(&arena);

  result = Token_Stream_init(&stream, 1024, getDefaultAllocator());
  if (!Result_isSuccess(result)) {
    Result_printData(result);
    goto onError;
  }
  result = Lexer_init(&lexer, CMAKE_SOURCE_DIR "/assets/main.gr",
                      getDefaultAllocator());
  if (!Result_isSuccess(result)) {
    Result_printData(result);
    goto onError;
  }

  for (size_t i = 0; i < stream.capacity; i += 1) {
    Token_Proxy proxy =
        Token_createProxy(&stream.ptr[i], &stream.length[i], &stream.row[i],
                          &stream.column[i], &stream.type[i]);
    result = Lexer_tokenize(&lexer, &proxy);
    stream.count += 1;
    if (Token_getProxyType(&proxy) == TOKEN_TYPE_END_OF_FILE) {
      break;
    }
    /// [%Row:%Column]: %.*Token -> %TokenType
    printf("[%3zu:%3zu]: %-15.*s -> %s\n", stream.row[i], stream.column[i],
           (int)stream.length[i], stream.ptr[i], Token_getType(stream.type[i]));
  }

  Parser parser = {
      .index = 0, .pStream = &stream, .pAllocator = &arenaAllocator};
  AST_Root *pRoot = Parser_parseAST(&parser);
  AST_DebugContext debugContext = {
      .indentation = 4, .depth = 0, .pStream = &stream};
  AST_Visitor debugVisitor = AST_getDebug(&debugContext);
  AST_accept((AST *)pRoot, &debugVisitor);

onError:
  Arena_release(&arena, getDefaultAllocator());
  Lexer_release(&lexer, getDefaultAllocator());
  Token_Stream_release(&stream, getDefaultAllocator());
  return 0;
}
