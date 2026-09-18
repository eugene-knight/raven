#ifndef AST_H
#define AST_H

#include "Token.h"

#define AST_TYPE__                                                             \
  X(AST_TYPE_UNDEFINED)                                                        \
  X(AST_TYPE_ROOT)                                                             \
  X(AST_TYPE_DATA_TYPE)                                                        \
  X(AST_TYPE_SIMPLE_TYPE)                                                      \
  X(AST_TYPE_POINTER_TYPE)                                                     \
  X(AST_TYPE_MUTABLE_TYPE)                                                     \
  X(AST_TYPE_FUNCTION_TYPE)                                                    \
  X(AST_TYPE_IDENTIFIER)                                                       \
  X(AST_TYPE_DECLARATION)                                                      \
  X(AST_TYPE_FUNCTION_DEFINITION)                                              \
  X(AST_TYPE_FUNCTION_DECLARATION)                                             \
  X(AST_TYPE_EXPRESSION_OPERATOR)                                              \
  X(AST_TYPE_EXPRESSION_BINARY_OPERATOR)                                       \
  X(AST_TYPE_EXPRESSION_LITERAL)

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

typedef struct AST_Root {
  struct AST base;
  AST *pAST;
} AST_Root;

typedef struct AST_Identifier {
  struct AST base;
  Token_Index name;
} AST_Identifier;

typedef struct AST_DataType {
  struct AST base;
} AST_DataType;

typedef struct AST_SimpleType {
  struct AST_DataType self;
  AST_Identifier name;
} AST_SimpleType;
typedef struct AST_PointerType {
  struct AST_DataType self;
  AST_DataType *pType;
} AST_PointerType;
typedef struct AST_MutableType {
  struct AST_DataType self;
  AST_DataType *pType;
} AST_MutableType;
typedef struct AST_FunctionType {
  struct AST_DataType self;
  AST_DataType *pParameter;
  AST_DataType *pReturnType;
} AST_FunctionType;

typedef struct AST_Declaration {
  struct AST base;
  AST_Identifier name;
  AST_DataType *pDataType;
} AST_Declaration;

typedef struct AST_Expression {
  struct AST base;
} AST_Expression;

typedef struct AST_FunctionDefinition {
  struct AST base;
  AST_Identifier name;
  AST_Identifier *pArguments;
  AST_Expression *pExpression;
} AST_FunctionDefinition;
typedef struct AST_VariableDefinition {
  struct AST base;
  AST_Identifier name;
  AST_DataType *pType;
  AST_Expression *pExpression;
} AST_VariableDefinition;

typedef uint32_t Literal_Type;
#define LITERAL_TYPE__ X(LITERAL_TYPE_INTEGER, "Integer")
enum {
#define X(ENUM, _) ENUM,
  LITERAL_TYPE__
#undef X
};
const char *const Literal_getPtr(Literal_Type type);
const char *const Literal_getType(Literal_Type type);

typedef struct AST_Literal {
  struct AST_Expression self;
  Literal_Type type;
  union {
    int integer;
  } as;
} AST_Literal;

typedef uint32_t Operator_Precedence;
#define OPERATOR_PRECEDENCE_MINIMUM                                            \
  2 /// < 1 for right-associativity, 0 for detecting errors
#define OPERATOR_PRECEDENCE__                                                  \
  X(TOKEN_TYPE_LEFT_PARENTHESIS, 130)                                          \
  X(TOKEN_TYPE_COLON_COLON, 60)                                                \
  X(TOKEN_TYPE_PERCENTAGE, 100)                                                \
  X(TOKEN_TYPE_FORWARD_SLASH, 100)                                             \
  X(TOKEN_TYPE_ASTERISK, 100)                                                  \
  X(TOKEN_TYPE_MINUS, 90)                                                      \
  X(TOKEN_TYPE_PLUS, 90)                                                       \
  X(TOKEN_TYPE_EQUAL, 10)                                                      \
  X(TOKEN_TYPE_LESS_THAN_MINUS, 10)                                            \
  X(TOKEN_TYPE_SEMICOLON, OPERATOR_PRECEDENCE_MINIMUM)

Operator_Precedence Operator_getMininum(void);
Operator_Precedence Operator_getPrecedence(Token_Type type);

#define OPERATOR_TYPE__                                                        \
  X(OPERATOR_TYPE_COLON_COLON, "::")                                           \
  X(OPERATOR_TYPE_LESS_THAN_MINUS, "<-")                                       \
  X(OPERATOR_TYPE_PERCENTAGE, "%")                                             \
  X(OPERATOR_TYPE_FORWARD_SLASH, "/")                                          \
  X(OPERATOR_TYPE_ASTERISK, "*")                                               \
  X(OPERATOR_TYPE_MINUS, "-")                                                  \
  X(OPERATOR_TYPE_PLUS, "+")                                                   \
  X(OPERATOR_TYPE_EQUAL, "=")

typedef uint32_t Operator_Type;
enum {
#define X(ENUM, _) ENUM,
  OPERATOR_TYPE__
#undef X
};
const char *const Operator_getType(Operator_Type op);
const char *const Operator_getPtr(Operator_Type op);

typedef struct AST_BinaryOperator {
  struct AST_Expression self;
  Operator_Type operatorType;
  AST_Expression *pLeft;
  AST_Expression *pRight;
} AST_BinaryOperator;

typedef struct AST_Call {
  struct AST_Expression self;
  AST_Expression *pArguments;
} AST_Call;

#endif
