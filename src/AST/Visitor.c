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

// void Debug_visitLiteral(AST_Literal *pLiteral, AST_Visitor *pVisitor) {
//   switch (pLiteral->type) {
//   case LITERAL_TYPE_INTEGER: {
//     DEBUG_LOG("AST_Literal: %d\n", pLiteral->as.integer);
//     break;
//   }
//   default:
//     assert(0 && "unimplemented case");
//   }
// }
//
void Debug_visitExpression(AST_Expression *pExpression, AST_Visitor *pVisitor) {
  // switch (pExpression->base.type) {
  // case AST_TYPE_EXPRESSION_LITERAL: {
  //   Debug_increaseDepth(pVisitor);
  //   Debug_visitLiteral((AST_Literal *)pExpression, pVisitor);
  //   Debug_decreaseDepth(pVisitor);
  //   break;
  // }
  // default:
  assert(0 && "case unimplemented");
  // }
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

      Result result = String_copyA(pString, " -> ", 4);
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
  DEBUG_LOG("AST_Declaration: %.*s\n",
            Debug_streamRange(pVisitor, pDeclaration->name.name,
                              pDeclaration->name.name),
            Debug_streamPtr(pVisitor, pDeclaration->name.name));
  AST_Type type = pDeclaration->base.type;
  DEBUG_LOG("+---| type: %s\n", AST_getType(type));

  switch (type) {
  case AST_TYPE_DECLARATION_FUNCTION: {
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
  default:
    assert(0 && "case unimplemented");
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
