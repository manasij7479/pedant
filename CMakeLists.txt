set(LLVM_LINK_COMPONENTS support)
set(LLVM_USED_LIBS clangTooling clangBasic clangAST)

add_clang_executable(pedant
  main.cpp
  )
target_link_libraries(pedant
  clangTooling
  clangBasic
  clangASTMatchers
  )