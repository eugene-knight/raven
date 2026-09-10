#include "Parser.h"

#define EXPECTS(TOKEN_TYPE) if ((pStream->type[parser->index] == TOKEN_TYPE))
#define EXPECTS_NOT(TOKEN_TYPE)                                                \
  if (!(pStream->type[parser->index] == TOKEN_TYPE))
#define NIL(EXPRESSION)                                                        \
  do {                                                                         \
    if (EXPRESSION) {                                                          \
      return NULL;                                                             \
    }                                                                          \
  } while (0)

NODISCARD
AST_Literal *parseLiteral(Parser *parser, Token_Stream *pStream,
                          const Allocator *const pAllocator) {
  AST_Literal *pLiteral = TALLOCATE(AST_Literal, 1);
  NIL(!pLiteral);

  pLiteral->base.type          = AST_TYPE_LITERAL;
  pLiteral->base.flags         = 0;
  pLiteral->base.pNext         = NULL;
  pLiteral->literal.base.type  = AST_TYPE_EXPRESSION;
  pLiteral->literal.base.flags = 0;
  pLiteral->literal.base.pNext = NULL;

  return pLiteral;
}

NODISCARD
AST_Expression *parseExpression(Parser *parser, Token_Stream *pStream,
                                const Allocator *const pAllocator);

NODISCARD
AST_Expression *parseFunctionBinding(Parser *parser, Token_Stream *pStream,
                                     const Allocator *const pAllocator) {
  assert(0 && "unimplemented");
  return NULL;
}

NODISCARD
AST_Expression *parseVariableBinding(Parser *parser, Token_Stream *pStream,
                                     const Allocator *const pAllocator) {
  assert(0 && "unimplemented");
  return NULL;
}

NODISCARD
AST_Expression *parseBinding(Parser *parser, Token_Stream *pStream,
                             const Allocator *const pAllocator) {
  AST_Binding *pBinding = TALLOCATE(AST_Binding, 1);
  NIL(!pBinding);

  pBinding->target.base.type  = AST_TYPE_BINDING;
  pBinding->target.base.pNext = NULL;
  pBinding->target.base.flags = 0;
  pBinding->target.range      = (Token_Range){0};
  pBinding->pBinder           = NULL;

  AST_Expression **ppExpression = &pBinding->pBinder;

  parser->index += 1;
  if (pStream->type[parser->index] == TOKEN_TYPE_LEFT_PARENTHESIS) {
    AST_Expression *pExpression = TALLOCATE(AST_Binding, 1);
    NIL(!pExpression);
    pExpression->base.type  = AST_TYPE_EXPRESSION;
    pExpression->base.pNext = NULL;
    pExpression->base.flags = 0;

    for (Bool shouldContinue = BOOL_TRUE; shouldContinue;) {
      parser->index += 1;

      uint32_t begin = parser->index;
      uint32_t end   = begin + 1;

      EXPECTS_NOT(TOKEN_TYPE_IDENTIFIER) {
        for (; pStream->ptr[parser->index][0] == '^' ||
               pStream->ptr[parser->index][0] == '$';) {
          parser->index += 1;
          end += 1;
        }
      }

      EXPECTS_NOT(TOKEN_TYPE_IDENTIFIER) {
        printf("%.*s\n", (int)pStream->length[parser->index],
               pStream->ptr[parser->index]);
        assert(0 && "value binding syntax error handling unimplemented");
      }

      pExpression->range = (Token_Range){
          .begin = begin,
          .end   = end,
      };

      *ppExpression = pExpression;
      ppExpression  = (AST_Expression **)&pExpression->base.pNext;

      parser->index += 1;
      EXPECTS_NOT(TOKEN_TYPE_COMMA) { shouldContinue = BOOL_FALSE; }
    }

    EXPECTS(TOKEN_TYPE_RIGHT_PARENTHESIS) {
      printf("%.*s\n", (int)pStream->length[parser->index],
             pStream->ptr[parser->index]);
      assert(0 && "value binding syntax error handling unimplemented");
    }

    parser->index += 1;
    EXPECTS(TOKEN_TYPE_SEMICOLON) {
      printf("%.*s\n", (int)pStream->length[parser->index],
             pStream->ptr[parser->index]);
      assert(0 && "value binding syntax error handling unimplemented");
    }

    pExpression = parseExpression(parser, pStream, pAllocator);
    NIL(!pExpression);
  }

  return (AST_Expression *)pBinding;
}

NODISCARD
AST_Expression *parseFunction(Parser *parser, Token_Stream *pStream,
                              const Allocator *const pAllocator) {
  AST_Function *pFunction = TALLOCATE(AST_Function, 1);
  NIL(!pFunction);

  pFunction->self.base.type  = AST_TYPE_FUNCTION;
  pFunction->self.base.flags = 0;
  pFunction->self.base.pNext = NULL;
  pFunction->pParameterTypes = NULL;
  pFunction->returnType      = (AST_Expression){0};

  AST_Expression **ppType = &pFunction->pParameterTypes;

  for (Bool shouldContinue = BOOL_TRUE; shouldContinue;) {
    parser->index += 1;
    AST_Expression *pParameter = TALLOCATE(AST_Expression, 1);
    NIL(!pParameter);

    pParameter->base.type  = AST_TYPE_EXPRESSION;
    pParameter->base.flags = 0;
    pParameter->base.pNext = NULL;

    uint32_t begin = parser->index;
    uint32_t end   = begin + 1;

    EXPECTS_NOT(TOKEN_TYPE_IDENTIFIER) {
      for (; pStream->ptr[parser->index][0] == '^' ||
             pStream->ptr[parser->index][0] == '$';) { ///< Pointer or constant
        parser->index += 1;
        end += 1;
      }
    }

    EXPECTS_NOT(TOKEN_TYPE_IDENTIFIER) {
      printf("%.*s\n", (int)pStream->length[parser->index],
             pStream->ptr[parser->index]);
      assert(0 && "function syntax error handling unimplemented");
    }

    pParameter->range = (Token_Range){
        .begin = begin,
        .end   = end,
    };

    *ppType = pParameter;
    ppType  = (AST_Expression **)&pParameter->base.pNext;

    parser->index += 1;
    EXPECTS_NOT(TOKEN_TYPE_COMMA) { shouldContinue = BOOL_FALSE; }
  }

  EXPECTS_NOT(TOKEN_TYPE_RIGHT_PARENTHESIS) {
    printf("%.*s\n", (int)pStream->length[parser->index],
           pStream->ptr[parser->index]);
    assert(0 && "function syntax error handling unimplemented");
  }

  parser->index += 1;
  EXPECTS_NOT(TOKEN_TYPE_MINUS_GREATER_THAN) {
    printf("%.*s\n", (int)pStream->length[parser->index],
           pStream->ptr[parser->index]);
    assert(0 && "function syntax error handling unimplemented");
  }

  parser->index += 1;
  EXPECTS_NOT(TOKEN_TYPE_IDENTIFIER) {
    printf("%.*s\n", (int)pStream->length[parser->index],
           pStream->ptr[parser->index]);
    assert(0 && "function syntax error handling unimplemented");
  }

  parser->index += 1;
  EXPECTS_NOT(TOKEN_TYPE_SEMICOLON) {
    printf("%.*s\n", (int)pStream->length[parser->index],
           pStream->ptr[parser->index]);
    assert(0 && "function syntax error handling unimplemented");
  }

  return (AST_Expression *)pFunction;
}

NODISCARD
AST_Expression *parseExpression(Parser *parser, Token_Stream *pStream,
                                const Allocator *const pAllocator) {
  AST_Expression *pExpression = NULL;
  parser->index += 1;
  if (pStream->type[parser->index] == TOKEN_TYPE_COLON_COLON) {

    parser->index += 1;
    if (pStream->type[parser->index] == TOKEN_TYPE_LEFT_PARENTHESIS) {
      pExpression = parseFunction(parser, pStream, pAllocator);
    } else {
      assert(0 && "declaration syntax error handling unimplemented");
    }
  } else if (pStream->type[parser->index] == TOKEN_TYPE_LESS_THAN_MINUS) {
    pExpression = parseBinding(parser, pStream, pAllocator);
  } else {
    assert(0 && "syntax error handling unimplemented");
  }

  parser->index += 1; ///< Advance past semicolons, starting a new expression
  return pExpression;
}

NODISCARD
AST_Root *Parser_parseAST(Parser *parser, Token_Stream *pStream,
                          const Allocator *const pAllocator) {
  AST_Root *pRoot = TALLOCATE(AST_Root, 1);
  NIL(!pRoot);

  pRoot->base.type  = AST_TYPE_ROOT;
  pRoot->base.flags = 0;
  pRoot->base.pNext = NULL;
  pRoot->children   = NULL;

  struct AST **pAST = &pRoot->children;

_restart:
  switch (pStream->type[parser->index]) {
  case TOKEN_TYPE_IDENTIFIER: {
    Token_Index nameIndex       = parser->index;
    AST_Expression *pExpression = parseExpression(parser, pStream, pAllocator);
    NIL(!pExpression);

    pExpression->range = (Token_Range){
        .begin = nameIndex,
        .end   = nameIndex + 1,
    };

    *pAST = (AST *)pExpression;
    pAST  = (AST **)&pExpression->base.pNext;

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
  static Debug_Context context   = {0};
  const static AST_Visitor debug = {
      .pfn_visitRoot       = Debug_visitRoot,
      .pfn_visitExpression = Debug_visitExpression,
      .pContext            = &context,
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
