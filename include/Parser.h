#ifndef PARSER_H
#define PARSER_H

#include "String.h"
#include "Token.h"

#define AST_TYPE__                                                             \
  X(AST_TYPE_UNDEFINED)                                                        \
  X(AST_TYPE_ROOT)                                                             \
  X(AST_TYPE_TYPE)                                                             \
  X(AST_TYPE_EXPRESSION)                                                       \
  X(AST_TYPE_LITERAL)                                                          \
  X(AST_TYPE_BINDING)                                                          \
  X(AST_TYPE_FUNCTION)

typedef uint32_t AST_Flags;
typedef uint32_t AST_Type;
enum {
#define X(ENUM) ENUM,
  AST_TYPE__
#undef X
};

typedef struct AST {
  struct AST *pNext;
  AST_Type type;
  AST_Flags flags;
} AST;

typedef struct AST_Root {
  struct AST base;
  struct AST *children;
} AST_Root;

typedef struct AST_Expression {
  struct AST base;
  Token_Range range;
} AST_Expression;

typedef struct AST_Function {
  struct AST_Expression self;
  AST_Expression *pParameterTypes;
  AST_Expression returnType;
} AST_Function;

typedef struct AST_Binding {
  struct AST_Expression target;
  AST_Expression *pBinder;
} AST_Binding;

typedef struct AST_Literal {
  struct AST base;
  AST_Expression literal;
} AST_Literal;

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
