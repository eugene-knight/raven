#include "Arena_Allocator.h"
#include "Lexer.h"
#include "Parser.h"

#include <stdint.h>

typedef const Allocator *const _Allocator;

int main(void) {
  Result result = {0};
  _Allocator alloc = &g_defaultAllocator;
  Lexer_initTables();

  Arena arena = {0};
  Token_Stream stream = {0};
  Lexer lexer = {0};
  Parser parser = {0};
  const Allocator aalloc = Arena_getAllocator(&arena);

  result = Arena_init(&arena, 1024 * 4, alloc);
  CHKRESULTDATA_ONERRORG(result);

  result = Token_Stream_init(&stream, 1024 * 4, alloc);
  CHKRESULTDATA_ONERRORG(result);

  result = Lexer_init(&lexer, "../assets/main.rvn", alloc);
  CHKRESULTDATA_ONERRORG(result);

  Lexer_populateTokenStream(&lexer, &stream);

  // printf("Tokens:\n");
  // for (size_t i = 0; stream.type[i] != TOKEN_TYPE_END_OF_FILE; i += 1) {
  //   printf("+----| %-15.*s -> %s\n", (int)stream.length[i], stream.ptr[i],
  //          Token_getType(stream.type[i]));
  // }

  AST_Root *pRoot = Parser_parseAST(&parser, &stream, &aalloc);
  AST_accept((AST *)pRoot, AST_Visitor_Debug());

_result:
  Arena_release(&arena, alloc);
  Token_Stream_release(&stream, alloc);
  Lexer_release(&lexer, alloc);

  return 0;
}
