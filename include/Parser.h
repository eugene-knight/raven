#ifndef PARSER_H
#define PARSER_H

#include "String.h"
#include "Token.h"

#define AST_TYPE__                                                             \
  X(AST_TYPE_UNDEFINED)                                                        \
  X(AST_TYPE_ROOT)                                                             \
  X(AST_TYPE_TYPE)                                                             \
  X(AST_TYPE_EXPRESSION)                                                       \
  X(AST_TYPE_EXPRESSION_FUNCTION)

typedef uint32_t AST_Type;
enum {
#define X(ENUM) ENUM,
  AST_TYPE__
#undef X
};

typedef struct AST {
  struct AST *pNext;
  AST_Type type;
} AST;

typedef struct AST_Root {
  struct AST base;
  struct AST *children;
} AST_Root;

struct AST_Type {
  struct AST base;
  String_View view;
};
typedef struct AST_Type AST_Kind;

typedef struct AST_Expression {
  struct AST base;
  String_View view;
} AST_Expression;

typedef struct AST_Expression_Function {
  struct AST_Expression self;
  struct AST_Type *parameterTypes;
  struct AST_Type returnType;
} AST_Expression_Function;

typedef struct Parser {
  size_t index;
} Parser;
AST_Root *Parser_parseAST(Parser *parser, Token_Stream *pStream,
                          const Allocator *const pAllocator);

typedef struct AST_Visitor {
  void (*pfn_visitRoot)(const AST *const pRoot,
                        const struct AST_Visitor *const pVisitor);
  void (*pfn_visitExpression)(const AST *const pExpression,
                              const struct AST_Visitor *const pVisitor);
  void *pContext;
} AST_Visitor;

void AST_accept(const AST *const pAST, const AST_Visitor *const pVisitor);
const AST_Visitor *AST_Visitor_Debug(void);

const char *const AST_getType(AST_Type type);

#endif
