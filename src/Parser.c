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

static AST_Definition *parseFunctionDefinition(Parser *parser) {

}

// static AST_Definition *parseFunctionDeclarationDefinition(Parser *parser,
//                                                Function *pTransaction) {
//   assert(0 && "unimplemented");
//   return NULL;
// }

static AST_Declaration *parseVariableDeclaration(Parser *parser,
                                                 Variable *pTransaction) {
  assert(0 && "unimplemented");
  return NULL;
}

static AST_DataType *parseDataType(Parser *parser);

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
  AST_setReference(&ref, &pFunction->pParameter);

  if (!Parser_expects(parser, TOKEN_TYPE_RIGHT_PARENTHESIS)) {
    for (;;) {
      AST_DataType *pParameter = parseDataType(parser);
      if (!pParameter) {
        return NULL;
      }
      AST_referenceSetValue(&ref, pParameter);
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

static AST_Declaration *parseFunctionDeclaration(Parser *parser,
                                                 Function *pTransaction) {
  pTransaction->pFunctionType = parseFunctionType(parser);
  if (!pTransaction->pFunctionType) {
    return NULL;
  }

  if (!Parser_expects(parser, TOKEN_TYPE_SEMICOLON)) {
    Parser_syntaxError(parser);
    // return (AST_Declaration *)parseFunctionDeclarationDefinition(parser,
    // pTransaction);
  }
  Parser_advance(parser);

  AST_Declaration *pFunction =
      TALLOCATE_(parser->pAllocator, AST_Declaration, 1);
  if (!pFunction) {
    return NULL;
  }
  *pFunction = (AST_Declaration){0};
  pFunction->base.type = AST_TYPE_DECLARATION_FUNCTION;
  pFunction->pDataType = pTransaction->pFunctionType;
  return pFunction;
}

static AST_Declaration *parseDeclaration(Parser *parser) {
  AST_Declaration *pDeclaration = NULL;
  size_t nameIndex = Parser_index(parser);

  Parser_advance(parser);
  if (Parser_expects(parser, TOKEN_TYPE_COLON_COLON)) {
    Parser_advance(parser);
    if (Parser_expects(parser, TOKEN_TYPE_LEFT_PARENTHESIS)) {
      Function fn = {0};
      fn.name.base.type = AST_TYPE_IDENTIFIER;
      fn.name.name = nameIndex;
      pDeclaration = parseFunctionDeclaration(parser, &fn);
      if (!pDeclaration) {
        return NULL;
      }
    } else {
      Variable var = {0};
      var.name.base.type = AST_TYPE_IDENTIFIER;
      var.name.name = nameIndex;
      pDeclaration = parseVariableDeclaration(parser, &var);
      if (!pDeclaration) {
        return NULL;
      }
      assert(0 && "variable declaration unimplemented");
    }
  } else if (Parser_expects(parser, TOKEN_TYPE_LEFT_PARENTHESIS)) {
    Parser_advance(parser);
    pDeclaration = (AST_Declaration *)parseFunctionDefinition(parser);
    pDeclaration->name.base.type = AST_TYPE_IDENTIFIER;
    pDeclaration->name.name = nameIndex;
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
  default:
    assert(0 && "case unimplemented");
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
  UNUSED(parseExpression);
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
