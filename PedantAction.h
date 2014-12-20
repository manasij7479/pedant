#ifndef PEDANT_ACTION_H
#define PEDANT_ACTION_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"

class PedantVisitor : public clang::RecursiveASTVisitor<PedantVisitor> {
public:
  bool VisitFunctionDecl(clang::FunctionDecl* Decl) {
    if ( Decl->getDeclName().isIdentifier())
      llvm::outs() << "Function:" << Decl->getName() << Decl->isCXXClassMember() << "\n"; // this part visibly works
    return true;
  }
  bool VisitVarDecl(clang::VarDecl* Decl) {
    if ( Decl->getDeclName().isIdentifier())
      llvm::outs()<< "VAR: " << Decl->getName() << "\n";
    return true;
  }
  bool VisitCXXRecordDecl(clang::CXXRecordDecl* Decl) {
    if ( Decl->getDeclName().isIdentifier())
      llvm::outs() << "Class:" << Decl->getName() <<"\n";
    //Decl->dump();
    return true;
  }
  bool VisitFieldDecl(clang::FieldDecl* Decl) {
    if ( Decl->getDeclName().isIdentifier())
      llvm::outs() << "Field :" << Decl->getName() <<"\n";
    return true;
  }

  bool VisitCXXMethodDecl(clang::CXXMethodDecl* Decl) {
    if ( Decl->getDeclName().isIdentifier())
      llvm::outs() << "Method :" << Decl->getName() <<"\n";
    return true;
  }

};

class PedantConsumer : public clang::ASTConsumer {
public:
  PedantConsumer(clang::CompilerInstance& CI):Compiler(CI){};

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    // Traversing the translation unit decl via a RecursiveASTVisitor
    // will visit all nodes in the AST.
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  // A RecursiveASTVisitor implementation.
  PedantVisitor Visitor;
  clang::CompilerInstance& Compiler;
};

class PedantAction : public clang::ASTFrontendAction {
public:
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
      return std::unique_ptr<clang::ASTConsumer>(
        new PedantConsumer(Compiler));
  }
};

#endif
