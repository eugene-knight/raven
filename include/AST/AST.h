#ifndef AST_H
#define AST_H

#include "Token.h"

#define AST_TYPE__                                                             \
  X(AST_TYPE_UNDEFINED)                                                        \
  X(AST_TYPE_ROOT)                                                             \
  X(AST_TYPE_DATA_TYPE)                                                        \
  X(AST_TYPE_IDENTIFIER)                                                       \
  X(AST_TYPE_DECLARATION)                                                      \
  X(AST_TYPE_DECLARATION_FUNCTION)                                             \
  X(AST_TYPE_DECLARATION_VARIABLE)                                             \
  X(AST_TYPE_DEFINITION)                                                       \
  X(AST_TYPE_DEFINITION_FUNCTION)                                              \
  X(AST_TYPE_DEFINITION_VARIABLE)                                              \
  X(AST_TYPE_DECLARATION_N_DEFINITION)                                         \
  X(AST_TYPE_DECLARATION_N_DEFINITION_FUNCTION)                                \
  X(AST_TYPE_DECLARATION_N_DEFINITION_VARIABLE)                                \
  X(AST_TYPE_EXPRESSION_STATEMENT)                                             \
  X(AST_TYPE_EXPRESSION_BINARY_OPERATION)                                      \
  X(AST_TYPE_EXPRESSION_LITERAL)

typedef uint32_t Operator_Precedence;
#define OPERATOR_PRECEDENCE__                                                  \
  X(TOKEN_TYPE_COLON_COLON, 60)                                                \
  X(TOKEN_TYPE_PLUS, 24)                                                       \
  X(TOKEN_TYPE_EQUAL, 10)                                                      \
  X(TOKEN_TYPE_SEMICOLON, 2)

Operator_Precedence Operator_getMininum(void);
Operator_Precedence Operator_getPrecedence(Token_Type type);

typedef uint32_t AST_Type;
enum {
#define X(ENUM) ENUM,
  AST_TYPE__
#undef X
};
const char *const AST_getType(AST_Type type);

typedef struct AST {
  struct AST *pNext;
  AST_Type type;
} AST;

typedef struct AST_Declaration AST_Declaration;

typedef struct AST_Root {
  struct AST base;
  AST_Declaration *pDeclarations;
} AST_Root;

typedef struct AST_Identifier {
  struct AST base;
  Token_Index name;
} AST_Identifier;

struct AST_Declaration {
  struct AST base;
  AST_Identifier name;
};

typedef struct AST_Definition {
  struct AST base;
  AST_Identifier name;
} AST_Definition;

typedef struct AST_Declaration_Definition {
  struct AST base;
  AST_Identifier name;
} AST_Declaration_Definition;

typedef struct AST_DataType {
  struct AST base;
  struct {
    Token_Index begin;
    Token_Index end;
  } range;
} AST_DataType;

typedef struct AST_Expression {
  struct AST base;
} AST_Expression;

typedef uint32_t Literal_Type;
#define LITERAL_TYPE__ X(LITERAL_TYPE_INTEGER)
enum {
#define X(ENUM) ENUM,
  LITERAL_TYPE__
#undef X
};

typedef struct AST_Literal {
  struct AST_Expression self;
  Literal_Type type;
  union {
    int integer;
  } as;
} AST_Literal;

typedef struct AST_Operator {
  struct AST base;
} AST_Operator;

typedef struct AST_BinaryOperation {
  struct AST_Expression self;
  AST_Operator operator;
  AST_Expression *pLeft;
  AST_Expression *pRight;
} AST_BinaryOperation;

typedef struct AST_Call {
  struct AST_Expression self;
  AST_Expression *pArguments;
} AST_Call;

typedef struct AST_Declaration_Function {
  struct AST_Declaration self;
  AST_DataType *pParameterTypes;
  AST_DataType *pReturnType;
} AST_Declaration_Function;

typedef struct AST_Definition_Function {
  struct AST_Definition self;
  AST_Identifier *pArguments;
  AST_Expression *pBody;
} AST_Definition_Function;

typedef struct AST_Declaration_Definition_Function {
  struct AST_Declaration_Definition self;
  AST_DataType *pParameterTypes;
  AST_DataType *pReturnType;
  AST_Identifier *pArguments;
  AST_Expression *pBody;
} AST_Declaration_Definition_Function;

typedef struct AST_Declaration_Variable {
  struct AST_Declaration self;
  AST_DataType *pType;
} AST_Declaration_Variable;

typedef struct AST_Definition_Variable {
  struct AST_Declaration_Variable super;
  AST_Expression *pInitializer;
} AST_Definition_Variable;

#endif
