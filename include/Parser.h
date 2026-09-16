#ifndef PARSER_H
#define PARSER_H

#include "AST/AST.h"
#include "Allocator.h"

typedef struct Parser {
  size_t index;
  Token_Stream *pStream;
  Allocator *pAllocator;
} Parser;

AST_Root *Parser_parseAST(Parser *parser);

#endif
