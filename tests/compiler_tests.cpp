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

std::string mon_exp = "(let ((x 0)) (let ((temp_0 (< x 3))) (if temp_0 2 4)))";
std::string input = "(let ((x 0)) (if (< x 3) 2 4))";

Expression *ast = Parser::parse(input);
Expression *anf = ToAnf::to_anf(ast);
std::string monadic_exp = anf->toString();

Instructions *ins = InstructionSelector::to_select(anf);
std::vector<std::vector<std::string>> *instructions_ = ins->get_instructions();


TEST_CASE("Compiler tests", "[compiler]") {

  REQUIRE(monadic_exp == mon_exp);

  for (size_t i = 0; i < (*instructions_).size(); i++) {
    for (size_t j = 0; j < (*instructions_)[i].size(); j++) {
      REQUIRE((*instructions_)[i][j] == (*instructions)[i][j]);
    }
  }
}
