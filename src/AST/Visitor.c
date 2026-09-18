#include "AST/Visitor.h"
#include "AST/AST.h"
#include "String.h"
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

// // static inline Token_Stream *Debug_getTokenStream(AST_Visitor *pVisitor) {
// //   AST_DebugContext *pContext = pVisitor->pContext;
// //   return pContext->pStream;
// // }
static inline char *Debug_streamPtr(AST_Visitor *pVisitor, size_t index) {
  AST_DebugContext *pContext = pVisitor->pContext;
  return Token_Stream_getPtr(pContext->pStream, index);
}
static inline size_t Debug_streamLength(AST_Visitor *pVisitor, size_t index) {
  AST_DebugContext *pContext = pVisitor->pContext;
  return pContext->pStream->length[index];
}
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

void Debug_visitExpression(AST_Expression *pExpression, AST_Visitor *pVisitor);

void Debug_visitBinaryOperator(AST_BinaryOperator *pBOP,
                               AST_Visitor *pVisitor) {
  DEBUG_LOG("+---| AST_BinaryOperator: %s\n",
            Operator_getPtr(pBOP->operatorType));
  Debug_increaseDepth(pVisitor);
  Debug_visitExpression(pBOP->pLeft, pVisitor);
  Debug_visitExpression(pBOP->pRight, pVisitor);
  Debug_decreaseDepth(pVisitor);
}
void Debug_visitLiteral(AST_Literal *pLiteral, AST_Visitor *pVisitor) {
  DEBUG_LOG("+---| AST_Literal: %s", "\n");
  Debug_increaseDepth(pVisitor);
  switch (pLiteral->type) {
  case LITERAL_TYPE_INTEGER: {
    DEBUG_LOG("+---| type: %s\n"
              "%-*s+---| value: %d\n",
              Literal_getPtr(pLiteral->type), Debug_getIndentation(pVisitor),
              "", pLiteral->as.integer);
    break;
  }

  default:
    fprintf(stderr, "[CASE]: %s\n", Literal_getType(pLiteral->type));
    assert(0 && "case unimplemented");
  }
  Debug_decreaseDepth(pVisitor);
}

void Debug_visitIdentifier(AST_Identifier *pID, AST_Visitor *pVisitor) {
  DEBUG_LOG("+---| name: %.*s\n",
            Debug_streamRange(pVisitor, pID->name, pID->name),
            Debug_streamPtr(pVisitor, pID->name));
}

void Debug_visitExpression(AST_Expression *pExpression, AST_Visitor *pVisitor) {
  switch (pExpression->base.type) {
  case AST_TYPE_IDENTIFIER: {
    Debug_visitIdentifier((AST_Identifier *)pExpression, pVisitor);
    break;
  }
  case AST_TYPE_EXPRESSION_BINARY_OPERATOR: {
    Debug_visitBinaryOperator((AST_BinaryOperator *)pExpression, pVisitor);
    break;
  }
  case AST_TYPE_EXPRESSION_LITERAL: {
    Debug_visitLiteral((AST_Literal *)pExpression, pVisitor);
    break;
  }
  default:
    assert(0 && "case unimplemented");
  }
}

void Debug_visitDataType(AST_DataType *pDataType, AST_Visitor *pVisitor,
                         String *pString) {
  AST_DataType *pCurrent = pDataType;
  for (;;) {
    switch (pCurrent->base.type) {
    case AST_TYPE_SIMPLE_TYPE: {
      AST_SimpleType *pSimple = (AST_SimpleType *)pCurrent;
      Result result =
          String_copyA(pString, Debug_streamPtr(pVisitor, pSimple->name.name),
                       Debug_streamLength(pVisitor, pSimple->name.name));
      if (!Result_isSuccess(result)) {
        Result_printData(result);
        abort();
      }
      pCurrent = NULL;
      break;
    }
    case AST_TYPE_POINTER_TYPE: {
      AST_PointerType *pPointer = (AST_PointerType *)pCurrent;
      Result result = String_copyA(pString, "^", 1);
      if (!Result_isSuccess(result)) {
        Result_printData(result);
        abort();
      }
      pCurrent = pPointer->pType;
      break;
    }
    case AST_TYPE_MUTABLE_TYPE: {
      AST_MutableType *pMutable = (AST_MutableType *)pCurrent;
      Result result = String_copyA(pString, "$", 1);
      if (!Result_isSuccess(result)) {
        Result_printData(result);
        abort();
      }
      pCurrent = pMutable->pType;
      break;
    }
    case AST_TYPE_FUNCTION_TYPE: {
      AST_FunctionType *pFunction = (AST_FunctionType *)pCurrent;
      AST_DataType *pParameter = pFunction->pParameter;
      Result result = String_copyA(pString, "(", 1);
      if (!Result_isSuccess(result)) {
        Result_printData(result);
        abort();
      }
      for (;;) {
        Debug_visitDataType(pParameter, pVisitor, pString);
        pParameter = (AST_DataType *)pParameter->base.pNext;
        if (!pParameter) {
          break;
        } else {
          Result result = String_copyA(pString, ", ", 2);
          if (!Result_isSuccess(result)) {
            Result_printData(result);
            abort();
          }
        }
      }
      result = String_copyA(pString, ") -> ", 5);
      if (!Result_isSuccess(result)) {
        Result_printData(result);
        abort();
      }
      pCurrent = pFunction->pReturnType;
      break;
    }
    default:
      assert(0 && "case unimplemented");
    }
    if (!pCurrent) {
      break;
    }
  }
}

void Debug_visitDeclaration(AST_Declaration *pDeclaration,
                            AST_Visitor *pVisitor) {
  AST_Type type = pDeclaration->base.type;
  switch (type) {
  case AST_TYPE_DECLARATION: {
    DEBUG_LOG("AST_Declaration: %.*s\n",
              Debug_streamRange(pVisitor, pDeclaration->name.name,
                                pDeclaration->name.name),
              Debug_streamPtr(pVisitor, pDeclaration->name.name));
    Debug_increaseDepth(pVisitor);
    AST_FunctionType *pFunction = (AST_FunctionType *)pDeclaration->pDataType;
    String string;
    Result result = String_init(&string, 1024, getDefaultAllocator());
    if (!Result_isSuccess(result)) {
      Result_printData(result);
      return;
    }
    Debug_visitDataType((AST_DataType *)pFunction, pVisitor, &string);
    DEBUG_LOG("+---| %.*s\n", (int)String_getLength(&string),
              String_getPtr(&string));
    String_release(&string, getDefaultAllocator());
    Debug_decreaseDepth(pVisitor);
    break;
  }
  case AST_TYPE_FUNCTION_DEFINITION: {
    AST_FunctionDefinition *pFD = (AST_FunctionDefinition *)pDeclaration;
    DEBUG_LOG("AST_FunctionDefinition: %.*s\n",
              Debug_streamRange(pVisitor, pDeclaration->name.name,
                                pDeclaration->name.name),
              Debug_streamPtr(pVisitor, pDeclaration->name.name));

    Debug_increaseDepth(pVisitor);
    AST_Identifier *pID = pFD->pArguments;
    DEBUG_LOG("+---| Parameter: %s", pID ? "\n" : "NONE");
    Debug_increaseDepth(pVisitor);
    for (; pID;) {
      DEBUG_LOG("+---| %.*s\n",
                Debug_streamRange(pVisitor, pID->name, pID->name),
                Debug_streamPtr(pVisitor, pID->name));
      pID = (AST_Identifier *)pID->base.pNext;
    }
    Debug_decreaseDepth(pVisitor);

    AST_Expression *pExpression = pFD->pExpression;
    DEBUG_LOG("+---| Expressions:%s", "\n");
    Debug_increaseDepth(pVisitor);
    for (; pExpression;) {
      Debug_visitExpression(pExpression, pVisitor);
      pExpression = (AST_Expression *)pExpression->base.pNext;
    }
    Debug_decreaseDepth(pVisitor);

    Debug_decreaseDepth(pVisitor);
    break;
  }
  default:
    assert(0 && "case unimplemented");
  }
}
void Debug_visitRoot(AST_Root *pRoot, AST_Visitor *pVisitor) {
  DEBUG_LOG("AST_Root:%s", "\n");

  Debug_increaseDepth(pVisitor);
  AST *pAST = pRoot->pAST;
  switch (pAST->type) {
  case AST_TYPE_DECLARATION: {
    for (; pAST;) {
      Debug_visitDeclaration((AST_Declaration *)pAST, pVisitor);
      pAST = pAST->pNext;
    }
    break;
  }
  default:
    assert(0 && "unimplemented");
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
