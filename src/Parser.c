#include "Parser.h"

static void syntaxError(void) { assert(0 && "unimplemented"); };

NODISCARD static AST_Definition *
parseDefinitionFunction(Parser *parser, Token_Stream *pStream,
                       const Allocator *const pAllocator) {
    assert(0 && "unimplemented");
    return NULL;
}

NODISCARD
static AST_Definition *
parseDefinitionVariable(Parser *parser, Token_Stream *pStream,
                        const Allocator *const pAllocator) {
  assert(0 && "unimplemented");
  return NULL;
}

NODISCARD
static AST_Declaration *
parseDeclarationFunction(Parser *parser, Token_Stream *pStream,
                         const Allocator *const pAllocator) {
  AST_Declaration_Function *fn = TALLOCATE(AST_Declaration_Function, 1);
  fn->self.base.type = AST_TYPE_DECLARATION_FUNCTION;
  fn->self.base.pNext = NULL;
  fn->parametersType = NULL;
  fn->returnType = (struct AST_Type){0};

  // === PARAMETERS === //
  parser->index += 1;
  struct AST **ppAST = (struct AST **)&fn->parametersType;
  for (;;) {
    if (pStream->type[parser->index] == TOKEN_TYPE_IDENTIFIER) {
      struct AST_Type *type = TALLOCATE(struct AST_Type, 1);
      if (!type) {
        return NULL;
      }

      type->base.type = AST_TYPE_TYPE;
      type->base.pNext = NULL;
      type->view = String_View_fromTokenStream(pStream, parser->index);

      *ppAST = (struct AST *)type;
      ppAST = &type->base.pNext;

      parser->index += 1;
      if (pStream->type[parser->index] == TOKEN_TYPE_COMMA) {
        parser->index += 1;
        continue;
      } else {
        break;
      }
    }
  }
  // === PARAMETERS === //

  // === RETURN TYPE === //
  if (pStream->type[parser->index] != TOKEN_TYPE_RIGHT_PARENTHESIS) {
    syntaxError();
  }
  parser->index += 1;
  if (pStream->type[parser->index] != TOKEN_TYPE_MINUS_GREATER_THAN) {
    syntaxError();
  }
  parser->index += 1;

  fn->returnType.base.type = AST_TYPE_TYPE;
  fn->returnType.base.pNext = NULL;
  fn->returnType.view = String_View_fromTokenStream(pStream, parser->index);
  parser->index += 1;
  // === RETURN TYPE === //

  // === TERMINATE DECLARATION === //
  if (pStream->type[parser->index] != TOKEN_TYPE_SEMICOLON) {
    syntaxError();
  }
  parser->index += 1;
  // === TERMINATE DECLARATION === //

  return (AST_Declaration *)fn;
}

NODISCARD
static AST *parseDeclarationDefinition(Parser *parser, Token_Stream *pStream,
                                       const Allocator *const pAllocator) {
  AST *pAST = NULL;
  size_t nameIndex = parser->index;

  parser->index += 1;
  switch (pStream->type[parser->index]) {
  case TOKEN_TYPE_COLON_COLON: {
    parser->index += 1;
    if (pStream->type[parser->index] == TOKEN_TYPE_LEFT_PARENTHESIS) {
      AST_Declaration *pDeclaration = (AST_Declaration *)pAST;
      pDeclaration = parseDeclarationFunction(parser, pStream, pAllocator);
      if (!pDeclaration) {
        return NULL;
      }
      pDeclaration->view = String_View_fromTokenStream(pStream, nameIndex);
      return (AST *)pDeclaration;
    }
    if (pStream->type[parser->index] == TOKEN_TYPE_IDENTIFIER ||
        pStream->type[parser->index] == TOKEN_TYPE_DIGIT) {
      AST_Definition *pDefinition = (AST_Definition *)pAST;
      pDefinition = parseDefinitionVariable(parser, pStream, pAllocator);
      if (!pDefinition) {
        return NULL;
      }
      pDefinition->view = String_View_fromTokenStream(pStream, nameIndex);
      return (AST *)pDefinition;
    }
  }
  case TOKEN_TYPE_IDENTIFIER: {
    AST_Definition *pDefinition = (AST_Definition *)pAST;
    pDefinition = parseDefinitionFunction(parser, pStream, pAllocator);
    if (!pDefinition) {
      return NULL;
    }
    pDefinition->view = String_View_fromTokenStream(pStream, nameIndex);
    break;
  }
  default:
    fprintf(stderr, "Token_Type: %s\n",
            Token_getType(pStream->type[parser->index]));
    assert(0 && "unimplemented || undefined");
  }

  return NULL;
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

  struct AST **ppAST = &pRoot->children;

_restart:
  switch (pStream->type[parser->index]) {
  case TOKEN_TYPE_IDENTIFIER: {
    AST_Declaration *pDeclaration =
        (AST_Declaration *)parseDeclarationDefinition(parser, pStream,
                                                      pAllocator);
    if (!pDeclaration) {
      return NULL;
    }

    *ppAST = (AST *)pDeclaration;
    ppAST = (AST **)&pDeclaration->base.pNext;

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
      pVisitor->pfn_visitRoot((AST_Root *)pAST, pVisitor);
    return;
  case AST_TYPE_DECLARATION:
    if (pVisitor->pfn_visitDeclarationDefinition != NULL)
      pVisitor->pfn_visitDeclarationDefinition(pAST, pVisitor);
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

void Debug_visitRoot(const AST_Root *const pRoot,
                     const AST_Visitor *const pVisitor);
void Debug_visitDeclarationDefinition(const AST *const pDeclaration,
                                      const AST_Visitor *const pVisitor);

NODISCARD
const AST_Visitor *AST_Visitor_Debug(void) {
  static Debug_Context context = {0};
  const static AST_Visitor debug = {
      .pfn_visitRoot = Debug_visitRoot,
      .pfn_visitDeclarationDefinition = Debug_visitDeclarationDefinition,
      .pContext = &context,
  };
  return &debug;
}

void Debug_visitRoot(const AST_Root *const pRoot,
                     const AST_Visitor *const pVisitor) {
  Debug_Context *pDebug = pVisitor->pContext;

  printf("AST_Root:\n");
  printf("+---| type: %s\n", AST_getType(pRoot->base.type));

  if (pRoot->children) {
    pDebug->indentation += 4;
    Debug_visitDeclarationDefinition(pRoot->children, pVisitor);
  }
}
void Debug_visitDeclarationDefinition(const AST *const pAST,
                                      const AST_Visitor *const pVisitor) {
  Debug_Context *pDebug = pVisitor->pContext;

  AST_Declaration *pDeclaration = (AST_Declaration *)pAST;

  switch (pDeclaration->base.type) {
  case AST_TYPE_DECLARATION_FUNCTION: {
    const AST_Declaration_Function *const fn =
        (const AST_Declaration_Function *const)pDeclaration;
    printf("%*s%s:\n", (int)pDebug->indentation, " ", "AST_Declaration");
    pDebug->indentation += 4;
    printf("%*s+---| name: %.*s\n", (int)pDebug->indentation, " ",
           (int)pDeclaration->view.length, pDeclaration->view.ptr);
    printf("%*s+---| type: %s\n", (int)pDebug->indentation, " ",
           AST_getType(pDeclaration->base.type));
    printf("%*s+---| Parameters\n", (int)pDebug->indentation, " ");
    pDebug->indentation += 4;

    ///< (void *) to get rid of const discard shenanigans
    ///< that is more trouble it is worth solving/worrying
    struct AST_Type **ppType = (void *)&fn->parametersType;

    for (size_t i = 0; *ppType != NULL; i += 1) {
      printf("%*s+---| %zu: %.*s\n", (int)pDebug->indentation, " ", i,
             (int)(*ppType)->view.length, (*ppType)->view.ptr);
      *ppType = (struct AST_Type *)(*ppType)->base.pNext;
    }
    pDebug->indentation -= 4;

    printf("%*s+---| return type: %.*s\n", (int)pDebug->indentation, " ",
           (int)fn->returnType.view.length, fn->returnType.view.ptr);
    break;
  }
  default:
    assert(0 && "unimplemented || undefined");
  };
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
