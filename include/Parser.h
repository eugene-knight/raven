#ifndef PARSER_H
#define PARSER_H

#include "String.h"
#include "Token.h"

#define AST_TYPE__                                                             \
  X(AST_TYPE_UNDEFINED)                                                        \
  X(AST_TYPE_ROOT)                                                             \
  X(AST_TYPE_TYPE)                                                             \
  X(AST_TYPE_DECLARATION)                                                      \
  X(AST_TYPE_DECLARATION_FUNCTION)                                             \
  X(AST_TYPE_DECLARATION_VARIABLE)                                             \
  X(AST_TYPE_DEFINITION_FUNCTION)                                              \
  X(AST_TYPE_DEFINITION_VARIABLE)

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

typedef struct AST_Declaration {
  struct AST base;
  String_View view;
} AST_Declaration;

typedef struct AST_Declaration_Function {
  struct AST_Declaration self;
  struct AST_Type *parametersType;
  struct AST_Type returnType;
} AST_Declaration_Function;
typedef struct AST_Declaration_Function AST_Decl_Fn;

typedef struct AST_Definition {
  struct AST base;
  String_View view;
} AST_Definition;

typedef struct AST_Definition_Function {
  struct AST_Definition self;
  struct AST_Type *identifiers;
  struct AST_Expression *expressions;
} AST_Definition_Function;
typedef struct AST_Definition_Function AST_Def_Fn;

typedef struct AST_Declaration_Variable {
  struct AST_Declaration self;
} AST_Declaration_Variable;
typedef struct AST_Declaration_Variable AST_Decl_Var;

typedef struct Parser {
  size_t index;
} Parser;
AST_Root *Parser_parseAST(Parser *parser, Token_Stream *pStream,
                          const Allocator *const pAllocator);

typedef struct AST_Visitor {
  void (*pfn_visitRoot)(const AST_Root *const pRoot,
                        const struct AST_Visitor *const pVisitor);
  void (*pfn_visitDeclarationDefinition)(
      const AST *const pDeclaration,
      const struct AST_Visitor *const pVisitor);

  void *pContext;
} AST_Visitor;

void AST_accept(const AST *const pAST, const AST_Visitor *const pVisitor);
const AST_Visitor *AST_Visitor_Debug(void);

const char *const AST_getType(AST_Type type);

#endif
