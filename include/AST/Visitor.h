#ifndef AST_VISITOR_H
#define AST_VISITOR_H

#include "AST/AST.h"

typedef struct AST_DebugContext {
  size_t indentation;
  size_t depth;
  Token_Stream *pStream;
} AST_DebugContext;

typedef struct AST_Visitor {
  void (*pfn_visitRoot)(AST_Root *pRoot, struct AST_Visitor *pVisitor);
  void (*pfn_visitDeclaration)(AST_Declaration *pDeclaration,
                               struct AST_Visitor *pVisitor);
  void (*pfn_visitExpression)(AST_Expression *pExpression,
                              struct AST_Visitor *pVisitor);
  void *pContext;
} AST_Visitor;

void AST_accept(AST *pAST, AST_Visitor *pVisitor);
AST_Visitor AST_getDebug(AST_DebugContext *pContext);

#endif
