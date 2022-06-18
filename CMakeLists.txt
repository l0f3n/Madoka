cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project (madoka VERSION 1.0)

set(
  SOURCES
  Main.cc
  Tokenizer/Tokenizer.cc
  Tokenizer/Token.cc
  Parser/Parser.cc
  AST/AST.cc
  AST/Print.cc
  SymbolTable/SymbolTable.cc
  SymbolTable/Symbol.cc
  Quads/Quads.cc
  CodeGenerator/CodeGenerator.cc
)

set(
  HEADERS
  Tokenizer/Tokenizer.h
  Tokenizer/Token.h
  Parser/Parser.h
  AST/AST.h
  SymbolTable/SymbolTable.h
  SymbolTable/Symbol.h
  Quads/Quads.h
  CodeGenerator/CodeGenerator.h
)

add_executable(madoka ${SOURCES} ${HEADERS})

target_include_directories(
  madoka
  PRIVATE .
)
