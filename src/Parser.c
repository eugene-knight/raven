#include "Parser.h"

NODISCARD
AST_Expression *
parseExpressionVariableTypeExplicit(Parser *parser, Token_Stream *pStream,
                                    const Allocator *const pAllocator) {
  assert(0 && "unimplemented");
  return NULL;
}
NODISCARD
AST_Expression *parseExpressionFunction(Parser *parser, Token_Stream *pStream,
                                        const Allocator *const pAllocator) {
  AST_Expression_Function *pFunction = TALLOCATE(AST_Expression_Function, 1);
  if (!pFunction) {
    return NULL;
  }
  pFunction->self.base.type = AST_TYPE_EXPRESSION_FUNCTION;
  pFunction->self.base.pNext = NULL;
  pFunction->parameterTypes = NULL;

  struct AST_Type **ppType = &pFunction->parameterTypes;

  parser->index += 1;
  for (; pStream->type[parser->index] == TOKEN_TYPE_IDENTIFIER;) {
    struct AST_Type *pType = TALLOCATE(struct AST_Type, 1);
    if (!pType) {
      return NULL;
    }

    *ppType = pType;
    ppType = (struct AST_Type **)&pType->base.pNext;

    char *start = pStream->ptr[parser->index];

  _restart:
    switch (pStream->type[parser->index + 1]) {
    case TOKEN_TYPE_CARET: {
      parser->index += 1;
      goto _restart;
    }
    }

    pType->base.type = AST_TYPE_TYPE;
    pType->base.pNext = NULL;
    pType->view = String_View_fromChar(
        start, ((pStream->ptr[parser->index] + pStream->length[parser->index]) -
                start));

    parser->index += 1;
    if (pStream->type[parser->index] != TOKEN_TYPE_COMMA) {
      break;
    } else {
      parser->index += 1;
    }
  }

  if (pStream->type[parser->index] != TOKEN_TYPE_RIGHT_PARENTHESIS) {
    printf("%.*s\n", (int)pStream->length[parser->index],
           pStream->ptr[parser->index]);
    assert(0 && "function syntax error handling unimplemented");
  }

  parser->index += 1;
  if (pStream->type[parser->index] != TOKEN_TYPE_MINUS_GREATER_THAN) {
    printf("%.*s\n", (int)pStream->length[parser->index],
           pStream->ptr[parser->index]);
    assert(0 && "function syntax error handling unimplemented");
  }

  parser->index += 1;
  if (pStream->type[parser->index] != TOKEN_TYPE_IDENTIFIER) {
    printf("%.*s\n", (int)pStream->length[parser->index],
           pStream->ptr[parser->index]);
    assert(0 && "function syntax error handling unimplemented");
  }

  parser->index += 1;
  if (pStream->type[parser->index] != TOKEN_TYPE_SEMICOLON) {
    printf("%.*s\n", (int)pStream->length[parser->index],
           pStream->ptr[parser->index]);
    assert(0 && "function syntax error handling unimplemented");
  }

  return (AST_Expression *)pFunction;
}
NODISCARD
AST_Expression *
parseExpressionVariableTypeInferred(Parser *parser, Token_Stream *pStream,
                                    const Allocator *const pAllocator) {
  assert(0 && "unimplemented");
  return NULL;
}

NODISCARD
AST_Expression *parseExpression(Parser *parser, Token_Stream *pStream,
                                const Allocator *const pAllocator) {
  AST_Expression *pExpression = NULL;
  parser->index += 1;
  if (pStream->type[parser->index] == TOKEN_TYPE_COLON_COLON) {
    parser->index += 1;
    if (pStream->type[parser->index] == TOKEN_TYPE_LEFT_PARENTHESIS) {
      pExpression = parseExpressionFunction(parser, pStream, pAllocator);
    } else {
      pExpression =
          parseExpressionVariableTypeExplicit(parser, pStream, pAllocator);
    }
  } else if (pStream->type[parser->index] == TOKEN_TYPE_COLON_EQUAL) {
    pExpression =
        parseExpressionVariableTypeInferred(parser, pStream, pAllocator);
  } else {
    assert(0 && "variable/function syntax error handling unimplemented");
  }

  parser->index += 1; ///< Advance past semicolons, starting a new expression
  return pExpression;
}

NODISCARD
AST_Root *Parser_parseAST(Parser *parser, Token_Stream *pStream,
                          const Allocator *const pAllocator) {
  AST_Root *pRoot = TALLOCATE(AST_Root, 1);
  if (!pRoot) {
    return NULL;
  }

  pRoot->base.type = AST_TYPE_ROOT;
  pRoot->base.pNext = NULL;
  pRoot->children = NULL;

  struct AST **pAST = &pRoot->children;

_restart:
  switch (pStream->type[parser->index]) {
  case TOKEN_TYPE_IDENTIFIER: {
    size_t nameIndex = parser->index;
    AST_Expression *pExpression = parseExpression(parser, pStream, pAllocator);
    if (!pExpression) {
      return NULL;
    }

    pExpression->view = String_View_fromTokenStream(pStream, nameIndex);

    *pAST = (AST *)pExpression;
    pAST = (AST **)pExpression->base.pNext;

    goto _restart;
  }
  case TOKEN_TYPE_END_OF_FILE: {
    break;
  }
  default:
    fprintf(stderr, "Token_Type: %s\n",
            Token_getType(pStream->type[parser->index]));
    assert(0 && "unimplemented || invalid");
  }

  return pRoot;
}

void AST_accept(const AST *const pAST, const AST_Visitor *const pVisitor) {
  assert(pAST && pVisitor);

  switch (pAST->type) {
  case AST_TYPE_ROOT:
    if (pVisitor->pfn_visitRoot != NULL)
      pVisitor->pfn_visitRoot(pAST, pVisitor);
    return;
  case AST_TYPE_EXPRESSION:
    if (pVisitor->pfn_visitExpression != NULL)
      pVisitor->pfn_visitExpression(pAST, pVisitor);
    return;
  case AST_TYPE_UNDEFINED:
    assert(0 && "Encountered an undefined AST");
  }
  fprintf(stderr, "%-10s: %s\n", "AST_Type", AST_getType(pAST->type));
  assert(0 && "Type was not assigned function pointer");
}

typedef struct Debug_Context {
  size_t indentation;
} Debug_Context;

typedef const AST *const _AST;
typedef const AST_Root *const _AST_Root;
typedef const AST_Expression *const _AST_Expression;

void Debug_visitRoot(_AST pAST, const AST_Visitor *const pVisitor);
void Debug_visitExpression(_AST pExpression, const AST_Visitor *const pVisitor);

NODISCARD
const AST_Visitor *AST_Visitor_Debug(void) {
  static Debug_Context context = {0};
  const static AST_Visitor debug = {
      .pfn_visitRoot = Debug_visitRoot,
      .pfn_visitExpression = Debug_visitExpression,
      .pContext = &context,
  };
  return &debug;
}

void Debug_visitExpression(_AST pExpression,
                           const AST_Visitor *const pVisitor) {
  assert(0 && "unimplemented");
}

void Debug_visitRoot(const AST *const pAST, const AST_Visitor *const pVisitor) {
  Debug_Context *pDebug = pVisitor->pContext;

  _AST_Root pRoot = (_AST_Root)pAST;

  printf("AST_Root:\n");
  printf("+---| type: %s\n", AST_getType(pRoot->base.type));

  if (pRoot->children) {
    pDebug->indentation += 4;
    Debug_visitExpression(pRoot->children, pVisitor);
  }
}
NODISCARD
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
