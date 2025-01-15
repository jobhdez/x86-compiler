#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include <cstdint>

#include "../parser.h"
#include "../toanf.h"
#include "../toselect.h"

std::vector<std::vector<std::string>> *instructions = new std::vector<std::vector<std::string>>{
  {"\t.globl ", "main\n"},
  {"main:\n"},
  {"\tpushq ", "%rbp\n"},
  {"\tmovq ", "%rsp, ", "%rbp\n"},
  {"\tsubq ", "$16, ", "%rsp\n"},
  {"\tmovq ", "$0, ", "-8(%rbp)\n"},
  {"\tcmpq ", "$3, ", "-8(%rbp)\n"},
  {"\tsetl ", "%al\n"},
  {"\tmovzbq ", "%al, ", "%rsi\n"},
  {"\tcmpq ", "$1, ", "%rsi\n"},
  {"\tje ", "block_16\n"},
  {"\tjmp ", "block_17\n"},
  {"\nblock_16:\n\n"},
  {"\tmovq ", "$2, ", "%rdi\n"},
  {"\tcallq ", "print_int\n"},
  {"\tjmp ", "conclusion\n"},
  {"\nblock_17:\n\n"},
  {"\tmovq ", "$4, ", "%rdi\n"},
  {"\tcallq ", "print_int\n"},
  {"\tjmp ", "conclusion\n"},
  {"\nconclusion:\n\n"},
  {"\taddq ", "$16, ", "%rsp\n"},
  {"\tpopq ", "%rbp\n"},
  {"\tretq"}};

std::vector<std::vector<std::string>> *input_instructions = new std::vector<std::vector<std::string>>{
  {"\t.globl ", "main\n"},
  {"main:\n"},
  {"\tpushq ", "%rbp\n"},
  {"\tmovq ", "%rsp, ", "%rbp\n"},
  {"\tsubq ", "$16, ", "%rsp\n"},
  {"\tmovq ", "$0, ", "-8(%rbp)\n"},
  {"\tmovq ", "$0, ", "-16(%rbp)\n"},
  {"\nloop_16:\n"},
  {"\tmovq ", "$3, ", "%rax\n"},
  {"\taddq ", "%rax, ", "-8(%rbp)\n"},
  {"\tmovq ", "$1, ", "%rax\n"},
  {"\taddq ", "%rax, ", "-16(%rbp)\n"},
  {"\tcmpq ", "$5, ", "-16(%rbp)\n"},
  {"\tjl ", "loop_16\n"},
  {"\tmovq ", "-8(%rbp)", " %rdi\n"},
  {"\tcallq ", "print_int\n"},
  {"\nconclusion:\n\n"},
  {"\taddq ", "$16, ", "%rsp\n"},
  {"\tpopq ", "%rbp\n"},
  {"\tretq"}};

std::string mon_exp = "(let ((x 0)) (let ((temp_0 (< x 3))) (if temp_0 2 4)))";
std::string input = "(let ((x 0)) (if (< x 3) 2 4))";
std::string input_ = "(let ((sum 0)) (let ((i 0)) (begin (while (< i 5) (begin (set sum (+ sum 3)) (set i (+ i 1)))) sum)))";

Expression *ast = Parser::parse(input);
Expression *anf = ToAnf::to_anf(ast);
std::string monadic_exp = anf->toString();

Instructions *ins = InstructionSelector::to_select(anf);
std::vector<std::vector<std::string>> *instructions_ = ins->get_instructions();

Expression *ast_exp = Parser::parse(input_);
Expression *anf_exp = ToAnf::to_anf(ast_exp);
std::string exp_mon = anf_exp->toString();

Instructions *instructs = InstructionSelector::to_select(anf_exp);
std::vector<std::vector<std::string>> *instructions__ = instructs->get_instructions();
TEST_CASE("Compiler tests", "[compiler]") {
  for (int i = 0; i < instructions__->size(); i++) {
        std::print("{}\n", (*instructions__)[i]);
    }

  REQUIRE(monadic_exp == mon_exp);

  for (size_t i = 0; i < (*instructions_).size(); i++) {
    for (size_t j = 0; j < (*instructions_)[i].size(); j++) {
      REQUIRE((*instructions_)[i][j] == (*instructions)[i][j]);
    }
  }

  for (size_t i = 0; i < (*instructions__).size(); i++) {
    for (size_t j = 0; j < (*instructions__)[i].size(); j++) {
      REQUIRE((*instructions__)[i][j] == (*input_instructions)[i][j]);
    }
  }

  
}
