#include "Parser.h"
#include <stdio.h>

typedef struct AST **AST_Reference;
static inline void AST_setReference(AST_Reference *pRef, AST **ppAST) {
  *pRef = ppAST;
}
static inline void AST_referenceSetValue(AST_Reference *pRef, AST *pAST) {
  **pRef = pAST;
}

static inline void Parser_advance(Parser *parser) { parser->index += 1; }
static inline size_t Parser_index(Parser *parser) { return parser->index; };
static inline char *Parser_ptr(Parser *parser) {
  return Token_Stream_getPtr(parser->pStream, parser->index);
}
static inline size_t Parser_length(Parser *parser) {
  return Token_Stream_getLength(parser->pStream, parser->index);
}
static inline Token_Type Parser_type(Parser *parser) {
  UNUSED(Parser_ptr);
  UNUSED(Parser_length);
  return Token_Stream_getType(parser->pStream, parser->index);
}
static inline Bool Parser_expects(Parser *parser, Token_Type type) {
  return (Parser_type(parser) == type);
}
// static inline Bool Parser_isDollarOrCaret(Parser *parser) {
//   return (Parser_type(parser) == TOKEN_TYPE_DOLLAR ||
//           Parser_type(parser) == TOKEN_TYPE_CARET);
// }

static void Parser_syntaxError(Parser *parser) { assert(0 && "unimplemented"); }

Operator_Precedence Operator_getMininum(void) { return 2; }
Operator_Precedence Operator_getPrecedence(Token_Type type) {
  switch (type) {
#define X(ENUM, VALUE)                                                         \
  case ENUM:                                                                   \
    return VALUE;
    OPERATOR_PRECEDENCE__
#undef X
  }
  return 0;
}

typedef struct Function {
  AST_Identifier name;
  AST_DataType *pFunctionType;
  AST_Expression *pExpressions;
} Function;
typedef struct Variable {
  AST_Identifier name;
  AST_DataType *pDataType;
  AST_Expression *pExpressions;
} Variable;

static AST_DataType *parseDataType(Parser *parser);
static AST_Expression *parseExpression(Parser *parser, Operator_Precedence op);

static AST_DataType *parseSimpleType(Parser *parser) {
  AST_SimpleType *pSimple = TALLOCATE_(parser->pAllocator, AST_SimpleType, 1);
  *pSimple = (AST_SimpleType){0};
  pSimple->self.base.type = AST_TYPE_SIMPLE_TYPE;
  pSimple->name.base.type = AST_TYPE_IDENTIFIER;
  pSimple->name.name = Parser_index(parser);
  Parser_advance(parser);
  return (AST_DataType *)pSimple;
}

static AST_DataType *parseMutableType(Parser *parser) {
  AST_MutableType *pMutable =
      TALLOCATE_(parser->pAllocator, AST_MutableType, 1);
  *pMutable = (AST_MutableType){0};
  pMutable->self.base.type = AST_TYPE_MUTABLE_TYPE;
  Parser_advance(parser);
  pMutable->pType = parseDataType(parser);
  return (AST_DataType *)pMutable;
}

static AST_DataType *parseFunctionType(Parser *parser) {
  AST_FunctionType *pFunction =
      TALLOCATE_(parser->pAllocator, AST_FunctionType, 1);
  *pFunction = (AST_FunctionType){0};
  pFunction->self.base.type = AST_TYPE_FUNCTION_TYPE;
  Parser_advance(parser);

  AST_Reference ref;
  AST_setReference(&ref, (AST **)&pFunction->pParameter);

  if (!Parser_expects(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
    for (;;) {
      AST_DataType *pParameter = parseDataType(parser);
      if (!pParameter) {
        return NULL;
      }
      AST_referenceSetValue(&ref, (AST *)pParameter);
      AST_setReference(&ref, &pParameter->base.pNext);
      if (!Parser_expects(parser, TOKEN_TYPE_COMMA)) {
        break;
      }
      Parser_advance(parser);
    }

    if (!Parser_expects(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
      Parser_syntaxError(parser);
    }
    Parser_advance(parser);
  }

  if (!Parser_expects(parser, TOKEN_TYPE_MINUS_GREATER_THAN)) {
    Parser_syntaxError(parser);
  }
  Parser_advance(parser);

  AST_DataType *pReturnType = parseDataType(parser);
  if (!pReturnType) {
    return NULL;
  }
  pFunction->pReturnType = pReturnType;

  return (AST_DataType *)pFunction;
}

static AST_DataType *parsePointerType(Parser *parser) {
  AST_PointerType *pPointer = TALLOCATE_(parser->pAllocator, AST_DataType, 1);
  *pPointer = (AST_PointerType){0};
  pPointer->self.base.type = AST_TYPE_POINTER_TYPE;
  Parser_advance(parser);
  pPointer->pType = parseDataType(parser);
  return (AST_DataType *)pPointer;
}

static AST_DataType *parseDataType(Parser *parser) {
  AST_DataType *pType = NULL;
  switch (Parser_type(parser)) {
  case TOKEN_TYPE_IDENTIFIER: {
    pType = parseSimpleType(parser);
    if (!pType) {
      return NULL;
    }
    pType->base.type = AST_TYPE_SIMPLE_TYPE;
    break;
  }
  case TOKEN_TYPE_CARET: {
    pType = parsePointerType(parser);
    if (!pType) {
      return NULL;
    }
    pType->base.type = AST_TYPE_POINTER_TYPE;
    break;
  }
  case TOKEN_TYPE_DOLLAR: {
    pType = parseMutableType(parser);
    if (!pType) {
      return NULL;
    }
    pType->base.type = AST_TYPE_MUTABLE_TYPE;
    break;
  }
  case TOKEN_TYPE_LEFT_PARENTHESIS: {
    pType = parseFunctionType(parser);
    if (!pType) {
      return NULL;
    }
    pType->base.type = AST_TYPE_FUNCTION_TYPE;

    break;
  }
  default:
    assert(0 && "unimplemented");
  }
  return pType;
}

static AST_FunctionDefinition *parseFunctionDefinition(Parser *parser) {
  AST_FunctionDefinition *pFD =
      TALLOCATE_(parser->pAllocator, AST_FunctionDefinition, 1);
  if (!pFD) {
    return NULL;
  }
  *pFD = (AST_FunctionDefinition){0};
  pFD->base.type = AST_TYPE_FUNCTION_DEFINITION;

  AST_Reference ref;
  AST_setReference(&ref, (AST **)&pFD->pArguments);
  for (;;) {
    AST_Identifier *pID = TALLOCATE_(parser->pAllocator, AST_Identifier, 1);
    if (!pID) {
      return NULL;
    }
    *pID = (AST_Identifier){0};
    pID->base.type = AST_TYPE_IDENTIFIER;
    pID->name = Parser_index(parser);
    Parser_advance(parser);
    AST_referenceSetValue(&ref, (AST *)pID);
    AST_setReference(&ref, &pID->base.pNext);
    if (!Parser_expects(parser, TOKEN_TYPE_COMMA)) {
      break;
    }
    Parser_advance(parser);
  }

  if (!Parser_expects(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
    Parser_syntaxError(parser);
  }
  Parser_advance(parser);

  if (!Parser_expects(parser, TOKEN_TYPE_EQUAL)) {
    Parser_syntaxError(parser);
  }
  Parser_advance(parser);

  AST_setReference(&ref, (AST **)&pFD->pExpression);
  for (;;) {
    AST_Expression *pExpression =
        parseExpression(parser, OPERATOR_PRECEDENCE_MINIMUM);
    if (!pExpression) {
      return NULL;
    }

    if (!Parser_expects(parser, TOKEN_TYPE_SEMICOLON)) {
      Parser_syntaxError(parser);
    }
    Parser_advance(parser);

    AST_referenceSetValue(&ref, (AST *)pExpression);
    AST_setReference(&ref, &pExpression->base.pNext);
    if (Parser_expects(parser, TOKEN_TYPE_KEYWORD_END)) {
      Parser_advance(parser);
      break;
    }
  }

  return pFD;
}

static AST_Declaration *parseDeclaration(Parser *parser) {
  AST_Declaration *pDeclaration =
      TALLOCATE_(parser->pAllocator, AST_Declaration, 1);
  if (!pDeclaration) {
    return NULL;
  }
  *pDeclaration = (AST_Declaration){0};
  // pDeclaration->name :: Set by Parser_parseAST
  pDeclaration->base.type = AST_TYPE_DECLARATION;
  pDeclaration->pDataType = parseDataType(parser);
  if (!Parser_expects(parser, TOKEN_TYPE_SEMICOLON)) {
    Parser_syntaxError(parser);
  }
  Parser_advance(parser);

  return pDeclaration;
}

static AST_Identifier *parseIdentifier(Parser *parser) {
  AST_Identifier *pID = TALLOCATE_(parser->pAllocator, AST_Identifier, 1);
  if (!pID) {
    return NULL;
  }
  *pID = (AST_Identifier){0};
  pID->base.type = AST_TYPE_IDENTIFIER;
  pID->name = Parser_index(parser);
  Parser_advance(parser);
  return pID;
}

static AST_Literal *parseLiteral(Parser *parser) {
  AST_Literal *pLiteral = TALLOCATE_(parser->pAllocator, AST_Literal, 1);
  if (!pLiteral) {
    return NULL;
  }
  *pLiteral = (AST_Literal){0};
  pLiteral->self.base.type = AST_TYPE_EXPRESSION_LITERAL;
  pLiteral->type = LITERAL_TYPE_INTEGER;

  char *start = Parser_ptr(parser);
  size_t length = Parser_length(parser);

  char c = start[length];
  start[length] = '\0';
  pLiteral->as.integer = strtol(start, NULL, 10);
  start[length] = c;
  Parser_advance(parser);

  return pLiteral;
}

static AST_BinaryOperator *
parseBinaryOperator(Parser *parser, Operator_Type type, AST_Expression *pLHS) {
  AST_BinaryOperator *pBOP =
      TALLOCATE_(parser->pAllocator, AST_BinaryOperator, 1);
  if (!pBOP) {
    return NULL;
  }
  *pBOP = (AST_BinaryOperator){0};
  pBOP->self.base.type = AST_TYPE_EXPRESSION_BINARY_OPERATOR;
  pBOP->pLeft = pLHS;

  switch (type) {
  case OPERATOR_TYPE_COLON_COLON: {
    pBOP->operatorType = OPERATOR_TYPE_COLON_COLON;
    pBOP->pRight =
        parseExpression(parser, Operator_getPrecedence(TOKEN_TYPE_COLON_COLON));
    break;
  }
  case OPERATOR_TYPE_EQUAL: {
    pBOP->operatorType = OPERATOR_TYPE_EQUAL;
    pBOP->pRight =
        parseExpression(parser, Operator_getPrecedence(TOKEN_TYPE_EQUAL) - 1);
    break;
  }
  case OPERATOR_TYPE_PLUS: {
    pBOP->operatorType = OPERATOR_TYPE_PLUS;
    pBOP->pRight =
        parseExpression(parser, Operator_getPrecedence(TOKEN_TYPE_PLUS));
    break;
  }
  case OPERATOR_TYPE_MINUS: {
    pBOP->operatorType = OPERATOR_TYPE_MINUS;
    pBOP->pRight =
        parseExpression(parser, Operator_getPrecedence(TOKEN_TYPE_MINUS));
    break;
  }
  case OPERATOR_TYPE_ASTERISK: {
    pBOP->operatorType = OPERATOR_TYPE_ASTERISK;
    pBOP->pRight =
        parseExpression(parser, Operator_getPrecedence(TOKEN_TYPE_ASTERISK));
    break;
  }
  case OPERATOR_TYPE_FORWARD_SLASH: {
    pBOP->operatorType = OPERATOR_TYPE_FORWARD_SLASH;
    pBOP->pRight = parseExpression(
        parser, Operator_getPrecedence(TOKEN_TYPE_FORWARD_SLASH));
    break;
  }
  case OPERATOR_TYPE_PERCENTAGE: {
    pBOP->operatorType = OPERATOR_TYPE_PERCENTAGE;
    pBOP->pRight =
        parseExpression(parser, Operator_getPrecedence(TOKEN_TYPE_PERCENTAGE));
    break;
  }
  case OPERATOR_TYPE_LESS_THAN_MINUS: {
    pBOP->operatorType = OPERATOR_TYPE_LESS_THAN_MINUS;
    pBOP->pRight = parseExpression(
        parser, Operator_getPrecedence(TOKEN_TYPE_LESS_THAN_MINUS) - 1);
    break;
  }
  default:
    assert(0 && "case unimplemented");
  }
  return pBOP;
}

static AST_Expression *parseLeftDenotation(Parser *parser,
                                           AST_Expression *pLHS) {
  AST_Expression *pExpression = NULL;
  switch (Parser_type(parser)) {
  case TOKEN_TYPE_LESS_THAN_MINUS: {
    Parser_advance(parser);
    pExpression = (AST_Expression *)parseBinaryOperator(
        parser, OPERATOR_TYPE_LESS_THAN_MINUS, pLHS);
    if (!pExpression) {
      return NULL;
    }
    break;
  }
  case TOKEN_TYPE_EQUAL: {
    Parser_advance(parser);
    pExpression = (AST_Expression *)parseBinaryOperator(
        parser, OPERATOR_TYPE_EQUAL, pLHS);
    if (!pExpression) {
      return NULL;
    }
    break;
  }
  case TOKEN_TYPE_PERCENTAGE: {
    Parser_advance(parser);
    pExpression = (AST_Expression *)parseBinaryOperator(
        parser, OPERATOR_TYPE_PERCENTAGE, pLHS);
    if (!pExpression) {
      return NULL;
    }
    break;
  }
  case TOKEN_TYPE_FORWARD_SLASH: {
    Parser_advance(parser);
    pExpression = (AST_Expression *)parseBinaryOperator(
        parser, OPERATOR_TYPE_FORWARD_SLASH, pLHS);
    if (!pExpression) {
      return NULL;
    }
    break;
  }
  case TOKEN_TYPE_ASTERISK: {
    Parser_advance(parser);
    pExpression = (AST_Expression *)parseBinaryOperator(
        parser, OPERATOR_TYPE_ASTERISK, pLHS);
    if (!pExpression) {
      return NULL;
    }
    break;
  }
  case TOKEN_TYPE_PLUS: {
    Parser_advance(parser);
    pExpression =
        (AST_Expression *)parseBinaryOperator(parser, OPERATOR_TYPE_PLUS, pLHS);
    if (!pExpression) {
      return NULL;
    }
    break;
  }
  case TOKEN_TYPE_MINUS: {
    Parser_advance(parser);
    pExpression = (AST_Expression *)parseBinaryOperator(
        parser, OPERATOR_TYPE_MINUS, pLHS);
    if (!pExpression) {
      return NULL;
    }
    break;
  }
  case TOKEN_TYPE_LEFT_PARENTHESIS: {
    Parser_advance(parser);
  }
  case TOKEN_TYPE_COLON_COLON: {
    Parser_advance(parser);
    pExpression = (AST_Expression *)parseBinaryOperator(
        parser, OPERATOR_TYPE_COLON_COLON, pLHS);
    if (!pExpression) {
      return NULL;
    }
    break;
  }
  default:
    assert(0 && "case unimplemented");
  }
  return pExpression;
}

static AST_Expression *parseNullDenotation(Parser *parser) {
  switch (Parser_type(parser)) {
  case TOKEN_TYPE_LEFT_PARENTHESIS: {
    Parser_advance(parser);
    AST_Expression *pExpression =
        parseExpression(parser, OPERATOR_PRECEDENCE_MINIMUM);
    if (!pExpression) {
      return NULL;
    }

    if (!Parser_expects(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
      Parser_syntaxError(parser);
    }
    return pExpression;
  }
  case TOKEN_TYPE_DIGIT: {
    AST_Literal *pLiteral = parseLiteral(parser);
    if (!pLiteral) {
      return NULL;
    }
    return (AST_Expression *)pLiteral;
  }
  case TOKEN_TYPE_IDENTIFIER: {
    AST_Identifier *pID = parseIdentifier(parser);
    if (!pID) {
      return NULL;
    }
    return (AST_Expression *)pID;
  }
  default:
    assert(0 && "case unimplemented");
  }
  return NULL;
}

static AST_Expression *parseExpression(Parser *parser, Operator_Precedence op) {
  AST_Expression *pLHS = parseNullDenotation(parser);
  for (;;) {
    Operator_Precedence newOp = Operator_getPrecedence(Parser_type(parser));
    assert(newOp != 0);
    if (newOp <= op) {
      break;
    }
    pLHS = parseLeftDenotation(parser, pLHS);
  }
  return pLHS;
}

AST_Root *Parser_parseAST(Parser *parser) {
  UNUSED(parseExpression);
  AST_Root *pRoot = TALLOCATE_(parser->pAllocator, AST_Root, 1);
  if (!pRoot) {
    return NULL;
  }
  *pRoot = (AST_Root){0};
  pRoot->base.type = AST_TYPE_ROOT;

  AST_Reference ref = NULL;
  AST_setReference(&ref, &pRoot->pAST);

  for (;;) {
    switch (Parser_type(parser)) {
    case TOKEN_TYPE_IDENTIFIER: {
      size_t nameIndex = Parser_index(parser);
      Parser_advance(parser);
      // INFO: Replace if/else chain with perfect hash (260918)

      if (!Parser_expects(parser, TOKEN_TYPE_COLON_COLON)) {
        if (!Parser_expects(parser, TOKEN_TYPE_LEFT_PARENTHESIS)) {
          Parser_syntaxError(parser);
        }
        Parser_advance(parser);
        AST_FunctionDefinition *pFD = parseFunctionDefinition(parser);
        pFD->name.base.type = AST_TYPE_IDENTIFIER;
        pFD->name.name = nameIndex;
        AST_referenceSetValue(&ref, (AST *)pFD);
        break;
      }
      Parser_advance(parser);

      AST_Declaration *pDeclaration = NULL;

      if (Parser_type(parser) == TOKEN_TYPE_KEYWORD_STRUCT) {
        assert(0 && "struct decl/def unimplemented");
      } else if (Parser_type(parser) == TOKEN_TYPE_KEYWORD_ENUM) {
        assert(0 && "enum decl/def unimplemented");
      } else if (Parser_type(parser) == TOKEN_TYPE_KEYWORD_UNION) {
        assert(0 && "union decl/def unimplemented");
      } else if (Parser_type(parser) == TOKEN_TYPE_KEYWORD_DATA) {
        assert(0 && "union decl/def unimplemented");
      } else {
        pDeclaration = parseDeclaration(parser);
        if (!pDeclaration) {
          return NULL;
        }
        pDeclaration->name.base.type = AST_TYPE_IDENTIFIER;
        pDeclaration->name.name = nameIndex;
        AST_referenceSetValue(&ref, (AST *)pDeclaration);
        AST_setReference(&ref, &pDeclaration->base.pNext);
      }
    } break;
    default:
      fprintf(stderr, "[CASE]: %s\n", Token_getType(Parser_type(parser)));
      assert(0 && "unimplemented token type");
    }
    if (Parser_type(parser) == TOKEN_TYPE_END_OF_FILE) {
      break;
    }
  }
  return pRoot;
}

const char *const AST_getType(AST_Type type) {
  switch (type) {
#define X(ENUM)                                                                \
  case ENUM:                                                                   \
    return #ENUM;
    AST_TYPE__
#undef X
  }
  return "AST_TYPE_UNKNOWN";
}
const char *const Operator_getType(Operator_Type op) {
  switch (op) {
#define X(ENUM, _)                                                             \
  case ENUM:                                                                   \
    return #ENUM;
    OPERATOR_TYPE__
#undef X
  }
  return "OPERATOR_TYPE_UNKNOWN";
}

const char *const Operator_getPtr(Operator_Type op) {
  switch (op) {
#define X(ENUM, CHAR)                                                          \
  case ENUM:                                                                   \
    return CHAR;
    OPERATOR_TYPE__
#undef X
  }
  return "OPERATOR_TYPE_UNKNOWN";
}

const char *const Literal_getType(Literal_Type type) {
  switch (type) {
#define X(ENUM, _)                                                             \
  case ENUM:                                                                   \
    return #ENUM;
    LITERAL_TYPE__
#undef X
  }
  return "LITERAL_TYPE_UNKNOWN";
}

const char *const Literal_getPtr(Literal_Type type) {
  switch (type) {
#define X(ENUM, CHAR)                                                          \
  case ENUM:                                                                   \
    return CHAR;
    LITERAL_TYPE__
#undef X
  }
  return "LITERAL_TYPE_UNKNOWN";
}
