#ifndef PEDANT_ACTION_H
#define PEDANT_ACTION_H

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/StmtVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Tooling/Tooling.h"
#include "Matchers.h"

class PedantVisitor : public clang::RecursiveASTVisitor<PedantVisitor> {
public:
  PedantVisitor(pedant::MatchHistory& mh) : m_Hist(mh){}
//  bool VisitFunctionDecl(clang::FunctionDecl* Decl) {
//    if ( Decl->getDeclName().isIdentifier())
//      llvm::outs() << "Function:" << Decl->getName() << Decl->isCXXClassMember() << "\n"; // this part visibly works
//    return true;
//  }
//  bool VisitVarDecl(clang::VarDecl* Decl) {
//    if ( Decl->getDeclName().isIdentifier())
//      llvm::outs()<< "VAR: " << Decl->getName() << "\n";
//    return true;
//  }
  bool VisitCXXRecordDecl(clang::CXXRecordDecl* Decl) {
    if ( Decl->getDeclName().isIdentifier()){
      m_Hist.matchName("class", Decl->getNameAsString());
    }
    return true;
  }
//  bool VisitFieldDecl(clang::FieldDecl* Decl) {
//    if ( Decl->getDeclName().isIdentifier())
//      llvm::outs() << "Field :" << Decl->getName() <<"\n";
//    return true;
//  }

//  bool VisitCXXMethodDecl(clang::CXXMethodDecl* Decl) {
//    if ( Decl->getDeclName().isIdentifier())
//      llvm::outs() << "Method :" << Decl->getName() <<"\n";
//    return true;
//  }
private:
  pedant::MatchHistory& m_Hist;

};

class PedantConsumer : public clang::ASTConsumer {
public:
  PedantConsumer(clang::CompilerInstance& CI, pedant::MatchHistory& mh)
      :Compiler(CI), Visitor(mh){}

  virtual void HandleTranslationUnit(clang::ASTContext &Context) {
    // Traversing the translation unit decl via a RecursiveASTVisitor
    // will visit all nodes in the AST.
    Visitor.TraverseDecl(Context.getTranslationUnitDecl());
  }
private:
  // A RecursiveASTVisitor implementation.
  clang::CompilerInstance& Compiler;
  PedantVisitor Visitor;
};

class PedantAction : public clang::ASTFrontendAction {
public:
  PedantAction(pedant::MatchHistory& mh):m_Hist(mh){}
  virtual std::unique_ptr<clang::ASTConsumer> CreateASTConsumer(
    clang::CompilerInstance &Compiler, llvm::StringRef InFile) {
      return std::unique_ptr<clang::ASTConsumer>(
        new PedantConsumer(Compiler, m_Hist));
  }
private:
  pedant::MatchHistory& m_Hist;
};

#endif
