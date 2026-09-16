#include "AST/Visitor.h"
#include "AST/AST.h"
#include <stdio.h>

static inline void Debug_increaseDepth(AST_Visitor *pVisitor) {
  AST_DebugContext *pContext = pVisitor->pContext;
  pContext->depth += 1;
}
static inline void Debug_decreaseDepth(AST_Visitor *pVisitor) {
  AST_DebugContext *pContext = pVisitor->pContext;
  pContext->depth -= 1;
}
static inline int Debug_getIndentation(AST_Visitor *pVisitor) {
  AST_DebugContext *pContext = pVisitor->pContext;
  return (int)(pContext->depth * pContext->indentation);
}
// static inline Token_Stream *Debug_getTokenStream(AST_Visitor *pVisitor) {
//   AST_DebugContext *pContext = pVisitor->pContext;
//   return pContext->pStream;
// }
static inline char *Debug_streamPtr(AST_Visitor *pVisitor, size_t index) {
  AST_DebugContext *pContext = pVisitor->pContext;
  return Token_Stream_getPtr(pContext->pStream, index);
}
// static inline size_t Debug_streamLength(AST_Visitor *pVisitor, size_t index)
// {
//   AST_DebugContext *pContext = pVisitor->pContext;
//   return pContext->pStream->length[index];
// }
// static inline size_t Debug_streamType(AST_Visitor *pVisitor, size_t index) {
//   AST_DebugContext *pContext = pVisitor->pContext;
//   return pContext->pStream->type[index];
// }
static inline int Debug_streamRange(AST_Visitor *pVisitor, size_t begin,
                                    size_t end) {
  AST_DebugContext *pContext = pVisitor->pContext;
  char *first = Token_Stream_getPtr(pContext->pStream, begin);
  char *second = Token_Stream_getPtr(pContext->pStream, end);
  size_t secondEnd = Token_Stream_getLength(pContext->pStream, end);

  return ((second + secondEnd) - first);
}

#define DEBUG_LOG(FMT, ...)                                                    \
  printf("%-*s" FMT, Debug_getIndentation(pVisitor), "", __VA_ARGS__)

void Debug_visitLiteral(AST_Literal *pLiteral, AST_Visitor *pVisitor) {
  switch (pLiteral->type) {
  case LITERAL_TYPE_INTEGER: {
    DEBUG_LOG("AST_Literal: %d\n", pLiteral->as.integer);
    break;
  }
  default:
    assert(0 && "unimplemented case");
  }
}

void Debug_visitExpression(AST_Expression *pExpression, AST_Visitor *pVisitor) {
  switch (pExpression->base.type) {
  case AST_TYPE_EXPRESSION_LITERAL: {
    Debug_increaseDepth(pVisitor);
    Debug_visitLiteral((AST_Literal *)pExpression, pVisitor);
    Debug_decreaseDepth(pVisitor);
    break;
  }
  default:
    assert(0 && "case unimplemented");
  }
}

void Debug_visitDeclaration(AST_Declaration *pDeclaration,
                            AST_Visitor *pVisitor) {
  DEBUG_LOG("AST_Declaration: %.*s\n",
            Debug_streamRange(pVisitor, pDeclaration->name, pDeclaration->name),
            Debug_streamPtr(pVisitor, pDeclaration->name));
  AST_Type type = pDeclaration->base.type;
  DEBUG_LOG("+---| type: %s\n", AST_getType(type));

  switch (type) {
  case AST_TYPE_DECLARATION_FUNCTION: {
    Debug_increaseDepth(pVisitor);
    AST_Declaration_Function *pFunction =
        (AST_Declaration_Function *)pDeclaration;
    AST_DataType *pParameter = pFunction->pParameterTypes;
    DEBUG_LOG("+---| Parameters: %s", pParameter ? "\n" : "NONE");
    for (; pParameter;) {
      DEBUG_LOG("+---| %.*s\n",
                Debug_streamRange(pVisitor, pParameter->range.begin,
                                  pParameter->range.end),
                Debug_streamPtr(pVisitor, pParameter->range.begin));
      pParameter = (AST_DataType *)pParameter->base.pNext;
    }
    AST_DataType *pReturnType = pFunction->pReturnType;
    DEBUG_LOG("+---| %.*s\n",
              Debug_streamRange(pVisitor, pReturnType->range.begin,
                                pReturnType->range.end),
              Debug_streamPtr(pVisitor, pReturnType->range.begin));
    Debug_decreaseDepth(pVisitor);
    break;
  }
  case AST_TYPE_DEFINITION_FUNCTION: {
    Debug_increaseDepth(pVisitor);
    AST_Definition_Function *pFunction =
        (AST_Definition_Function *)pDeclaration;
    AST_Identifier *pID = pFunction->pArguments;
    DEBUG_LOG("+---| Arguments: %s", pID ? "\n" : "NONE");
    Debug_increaseDepth(pVisitor);
    for (; pID;) {
      DEBUG_LOG("+---| %.*s\n",
                Debug_streamRange(pVisitor, pID->name, pID->name),
                Debug_streamPtr(pVisitor, pID->name));
      pID = (AST_Identifier *)pID->base.pNext;
    }
    Debug_decreaseDepth(pVisitor);
    DEBUG_LOG("+---| Body:%s", "\n");
    AST_Expression *pExpression = pFunction->pBody;
    for (; pExpression;) {
      Debug_visitExpression(pExpression, pVisitor);
      pExpression = (AST_Expression *)pExpression->base.pNext;
    }
    Debug_decreaseDepth(pVisitor);
    break;
  }
  case AST_TYPE_DECLARATION_VARIABLE: {
    assert(0 && "unimplemented");
    break;
  }
  default:
    assert(0 && "unreachaable");
  }
}
void Debug_visitRoot(AST_Root *pRoot, AST_Visitor *pVisitor) {
  DEBUG_LOG("AST_Root:%s", "\n");

  Debug_increaseDepth(pVisitor);
  AST_Declaration *pDeclaration = pRoot->pDeclarations;
  for (; pDeclaration;) {
    Debug_visitDeclaration(pDeclaration, pVisitor);
    pDeclaration = (AST_Declaration *)pDeclaration->base.pNext;
  }
  Debug_decreaseDepth(pVisitor);
}

AST_Visitor AST_getDebug(AST_DebugContext *pContext) {
  return (AST_Visitor){
      .pfn_visitRoot = Debug_visitRoot,
      .pfn_visitDeclaration = Debug_visitDeclaration,
      .pfn_visitExpression = Debug_visitExpression,
      .pContext = pContext,
  };
}

void AST_accept(AST *pAST, AST_Visitor *pVisitor) {
  assert(pAST && pVisitor);

  switch (pAST->type) {
  case AST_TYPE_ROOT: {
    Debug_visitRoot((AST_Root *)pAST, pVisitor);
    break;
  }
  case AST_TYPE_DECLARATION: {
    Debug_visitDeclaration((AST_Declaration *)pAST, pVisitor);
    break;
  }
  default:
    assert(0 && "unimplemented");
  }
}
