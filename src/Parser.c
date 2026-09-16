#include "Parser.h"
#include <stdio.h>

typedef struct AST **AST_Reference;
static inline void AST_setReference(AST_Reference *pRef, void *ppAST) {
  *pRef = ppAST;
}
static inline void AST_referenceSetValue(AST_Reference *pRef, void *pAST) {
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
static inline Bool Parser_DollarORCaret(Parser *parser) {
  return (Parser_type(parser) == TOKEN_TYPE_DOLLAR ||
          Parser_type(parser) == TOKEN_TYPE_CARET);
}
static inline Bool Parser_expects(Parser *parser, Token_Type type) {
  return (Parser_type(parser) == type);
}

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

static AST_Expression *parseExpression(Parser *parser, Operator_Precedence op);

typedef struct Function_Transaction {
  Token_Index name;
  AST_DataType *pParameterTypes;
  AST_DataType *pReturnType;
} Function_Transaction;
typedef struct Variable_Transaction {
  Token_Index name;
  AST_DataType *pDataType;
  AST_Expression *pInitializer;
} Variable_Transaction;

static AST_Declaration_Definition *
parseVariableDeclarationDefinition(Parser *parser,
                                   Variable_Transaction *trans) {
  assert(0 && "unimplemented");
  return NULL;
}

static AST_Declaration_Definition *
parseFunctionDeclarationDefinition(Parser *parser,
                                   Function_Transaction *trans) {
  AST_Declaration_Definition_Function *pFunction =
      TALLOCATE_(parser->pAllocator, AST_Declaration_Definition_Function, 1);
  if (!pFunction) {
    return NULL;
  }
  *pFunction = (AST_Declaration_Definition_Function){0};
  pFunction->self.base.type = AST_TYPE_DECLARATION_N_DEFINITION_FUNCTION;

  if (!Parser_expects(parser, TOKEN_TYPE_LESS_THAN_MINUS)) {
    Parser_syntaxError(parser);
  }
  Parser_advance(parser);

  if (!Parser_expects(parser, TOKEN_TYPE_LEFT_PARENTHESIS)) {
    Parser_syntaxError(parser);
  }
  Parser_advance(parser);

  if (!Parser_expects(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
    AST_Reference ref;
    AST_setReference(&ref, &pFunction->pArguments);
    for (;;) {
      AST_Identifier *pArguments =
          TALLOCATE_(parser->pAllocator, AST_Identifier, 1);
      if (!pArguments) {
        return NULL;
      }
      *pArguments = (AST_Identifier){0};
      pArguments->base.type = AST_TYPE_IDENTIFIER;
      pArguments->name = Parser_index(parser);

      AST_referenceSetValue(&ref, pArguments);
      AST_setReference(&ref, &pArguments->base.pNext);

      Parser_advance(parser);
      if (!Parser_expects(parser, TOKEN_TYPE_COMMA)) {
        break;
      }
      Parser_advance(parser);
    }
  }

  if (!Parser_expects(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
    Parser_syntaxError(parser);
  }
  Parser_advance(parser);

  if (!Parser_expects(parser, TOKEN_TYPE_EQUAL)) {
    Parser_syntaxError(parser);
  }
  Parser_advance(parser);

  AST_Reference ref;
  AST_setReference(&ref, &pFunction->pBody);
  for (;;) {
    AST_Expression *pExpression =
        parseExpression(parser, Operator_getMininum());
    if (!Parser_expects(parser, TOKEN_TYPE_SEMICOLON)) {
      Parser_syntaxError(parser);
    }
    Parser_advance(parser);
    if (!pExpression) {
      return NULL;
    }
    AST_referenceSetValue(&ref, pExpression);
    AST_setReference(&ref, &pExpression->base.pNext);
    if (Parser_type(parser) == TOKEN_TYPE_KEYWORD_END) {
      Parser_advance(parser);
      break;
    }
  }

  pFunction->self.name = trans->name;
  pFunction->pParameterTypes = trans->pParameterTypes;
  pFunction->pReturnType = trans->pReturnType;

  return (AST_Declaration_Definition *)pFunction;
}

static AST_Definition *parseFunctionDefinition(Parser *parser) {
  AST_Definition_Function *pFunction =
      TALLOCATE_(parser->pAllocator, AST_Definition_Function, 1);
  *pFunction = (AST_Definition_Function){0};
  pFunction->self.base.type = AST_TYPE_DEFINITION_FUNCTION;

  if (!Parser_expects(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
    AST_Reference ref;
    AST_setReference(&ref, &pFunction->pArguments);
    for (;;) {
      AST_Identifier *pID = TALLOCATE_(parser->pAllocator, AST_Identifier, 1);
      *pID = (AST_Identifier){0};
      pID->base.type = AST_TYPE_IDENTIFIER;
      pID->name = Parser_index(parser);

      AST_referenceSetValue(&ref, pID);
      AST_setReference(&ref, &pID->base.pNext);
      Parser_advance(parser);
      if (!Parser_expects(parser, TOKEN_TYPE_COMMA)) {
        break;
      }
      Parser_advance(parser);
    }
  }

  if (!Parser_expects(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
    Parser_syntaxError(parser);
  }
  Parser_advance(parser);

  if (!Parser_expects(parser, TOKEN_TYPE_EQUAL)) {
    Parser_syntaxError(parser);
  }
  Parser_advance(parser);

  AST_Reference ref;
  AST_setReference(&ref, &pFunction->pBody);
  for (;;) {
    AST_Expression *pExpression =
        parseExpression(parser, Operator_getMininum());
    if (!Parser_expects(parser, TOKEN_TYPE_SEMICOLON)) {
      Parser_syntaxError(parser);
    }
    Parser_advance(parser);
    AST_referenceSetValue(&ref, pExpression);
    AST_setReference(&ref, &pExpression->base.pNext);
    if (Parser_type(parser) == TOKEN_TYPE_KEYWORD_END) {
      Parser_advance(parser);
      break;
    }
  }

  return (AST_Definition *)pFunction;
}

static AST_Declaration *parseVariableDeclaration(Parser *parser,
                                                 Variable_Transaction *trans) {
  AST_DataType *pDataType = TALLOCATE_(parser->pAllocator, AST_DataType, 1);
  if (!pDataType) {
    return NULL;
  }
  *pDataType = (AST_DataType){0};
  pDataType->base.type = AST_TYPE_DECLARATION_VARIABLE;
  pDataType->range.begin = Parser_index(parser);

  for (; Parser_DollarORCaret(parser);) {
    Parser_advance(parser);
    if (Parser_expects(parser, TOKEN_TYPE_SEMICOLON)) {
      pDataType->range.end = Parser_index(parser);

      AST_Declaration_Variable *pVariable =
          TALLOCATE_(parser->pAllocator, AST_Declaration_Variable, 1);
      if (!pVariable) {
        return NULL;
      }
      *pVariable = (AST_Declaration_Variable){0};
      pVariable->self.base.type = AST_TYPE_DECLARATION_VARIABLE;
      pVariable->pType = pDataType;
      return (AST_Declaration *)pVariable;
    }
    if (Parser_expects(parser, TOKEN_TYPE_EQUAL)) {
      pDataType->range.end = Parser_index(parser);
      trans->pDataType = pDataType;
      return (AST_Declaration *)parseVariableDeclarationDefinition(parser,
                                                                   trans);
    }
  }
  return NULL;
}

static AST_Declaration *parseFunctionDeclaration(Parser *parser,
                                                 Function_Transaction *trans) {
  AST_Reference ref;
  AST_setReference(&ref, &trans->pParameterTypes);

  if (!Parser_expects(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
    for (;;) {
      AST_DataType *pAST = TALLOCATE_(parser->pAllocator, AST_DataType, 1);
      if (!pAST) {
        return NULL;
      }
      *pAST = (AST_DataType){0};
      pAST->base.type = AST_TYPE_DATA_TYPE;
      pAST->range.begin = Parser_index(parser);

      if (!Parser_expects(parser, TOKEN_TYPE_IDENTIFIER)) {
        for (; Parser_type(parser) == TOKEN_TYPE_CARET ||
               Parser_type(parser) == TOKEN_TYPE_DOLLAR;) {
          Parser_advance(parser);
        }
      }

      if (!Parser_expects(parser, TOKEN_TYPE_IDENTIFIER)) {
        Parser_syntaxError(parser);
      }

      pAST->range.end = Parser_index(parser);

      AST_referenceSetValue(&ref, pAST);
      AST_setReference(&ref, &pAST->base.pNext);

      Parser_advance(parser);
      if (!Parser_expects(parser, TOKEN_TYPE_COMMA)) {
        break;
      }
      Parser_advance(parser);
    }
  }

  if (!Parser_expects(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
    Parser_syntaxError(parser);
  }

  Parser_advance(parser);
  if (!Parser_expects(parser, TOKEN_TYPE_MINUS_GREATER_THAN)) {
    Parser_syntaxError(parser);
  }

  Parser_advance(parser);
  AST_DataType *pReturnType = TALLOCATE_(parser->pAllocator, AST_DataType, 1);
  if (!pReturnType) {
    return NULL;
  }

  *pReturnType = (AST_DataType){0};
  pReturnType->base.type = AST_TYPE_DATA_TYPE;
  pReturnType->range.begin = Parser_index(parser);

  if (!Parser_expects(parser, TOKEN_TYPE_IDENTIFIER)) {
    for (; Parser_type(parser) == TOKEN_TYPE_CARET ||
           Parser_type(parser) == TOKEN_TYPE_DOLLAR;) {
      Parser_advance(parser);
    }
  }

  if (!Parser_expects(parser, TOKEN_TYPE_IDENTIFIER)) {
    Parser_syntaxError(parser);
  }
  pReturnType->range.end = Parser_index(parser);

  Parser_advance(parser);
  if (!Parser_expects(parser, TOKEN_TYPE_SEMICOLON)) {
    Parser_advance(parser);
    return (AST_Declaration *)parseFunctionDeclarationDefinition(parser, trans);
  }

  Parser_advance(parser);
  AST_Declaration_Function *pDeclaration =
      TALLOCATE_(parser->pAllocator, AST_Declaration_Function, 1);
  *pDeclaration = (AST_Declaration_Function){0};
  pDeclaration->self.base.type = AST_TYPE_DECLARATION_FUNCTION;
  pDeclaration->self.name.base.type = AST_TYPE_IDENTIFIER;
  pDeclaration->self.name.name = trans->name;
  pDeclaration->pParameterTypes = trans->pParameterTypes;
  pDeclaration->pReturnType = pReturnType;

  return (AST_Declaration *)pDeclaration;
}

static AST_Declaration *parseDeclaration(Parser *parser) {
  AST_Declaration *pDeclaration = NULL;
  size_t nameIndex = Parser_index(parser);

  Parser_advance(parser);
  if (Parser_expects(parser, TOKEN_TYPE_COLON_COLON)) {
    Parser_advance(parser);
    if (Parser_expects(parser, TOKEN_TYPE_LEFT_PARENTHESIS)) {
      Parser_advance(parser);
      Function_Transaction trans = {0};
      trans.name = nameIndex;
      pDeclaration = parseFunctionDeclaration(parser, &trans);
      if (!pDeclaration) {
        return NULL;
      }
    } else {
      Parser_advance(parser);
      Variable_Transaction trans = {0};
      trans.name = nameIndex;
      pDeclaration = parseVariableDeclaration(parser, &trans);
      if (!pDeclaration) {
        return NULL;
      }
      assert(0 && "variable declaration unimplemented");
    }
  } else if (Parser_expects(parser, TOKEN_TYPE_LEFT_PARENTHESIS)) {
    Parser_advance(parser);
    pDeclaration = (AST_Declaration *)parseFunctionDefinition(parser);
    pDeclaration->name = nameIndex;
  } else {
    Parser_syntaxError(parser);
  }

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

static AST_Expression *parseLeftDenotation(Parser *parser) {
  switch (Parser_type(parser)) {
  case TOKEN_TYPE_COLON_COLON: {
    AST_Declaration *pDeclaration = parseVariableDeclaration(parser);
    if (!pDeclaration) {
      return NULL;
    }
    break;
  }
  default:
    assert(0 && "case unimplemented")
  }
  return NULL;
}

static AST_Expression *parseNullDenotation(Parser *parser) {
  switch (Parser_type(parser)) {
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
  AST_Expression *lhs = parseNullDenotation(parser);
  for (;;) {
    Operator_Precedence newOp = Operator_getPrecedence(Parser_type(parser));
    assert(newOp != 0);
    if (newOp <= op) {
      break;
    }
    lhs = parseLeftDenotation(parser);
  }
  return lhs;
}

AST_Root *Parser_parseAST(Parser *parser) {
  AST_Root *pRoot = TALLOCATE_(parser->pAllocator, AST_Root, 1);
  if (!pRoot) {
    return NULL;
  }
  *pRoot = (AST_Root){0};
  pRoot->base.type = AST_TYPE_ROOT;

  AST_Reference ref = NULL;
  AST_setReference(&ref, &pRoot->pDeclarations);

  switch (Parser_type(parser)) {
  case TOKEN_TYPE_IDENTIFIER: {
    for (; !Parser_expects(parser, TOKEN_TYPE_END_OF_FILE);) {
      AST_Declaration *pDeclaration = parseDeclaration(parser);
      if (!pDeclaration) {
        return NULL;
      }
      AST_referenceSetValue(&ref, pDeclaration);
      AST_setReference(&ref, &pDeclaration->base.pNext);
    }
    break;
  }
  default:
    assert(0 && "unimplemented token type");
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
