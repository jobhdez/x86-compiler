#include "parser.h"
#include "toanf.h"
#include "toselect.h"

int main() {
  //std::string input = "(let ((x 0)) (let ((y 8)) (if (< y 2) (if (< x 3) 1 2) 3)))";
  //  std::string input = "(let ((sum 0)) (let ((i 0)) (begin (while (< i 5) (begin (set sum (+ sum 1)) (set i (+ i 1)))) sum)))";
  std::string input = "(let ((sum 0)) (let ((i 0)) (begin (while (< i 5) (begin (set sum (+ sum i)) (set i (+ i 1)))) sum)))";
  Expression *ast = Parser::parse(input);
  Expression *anf = ToAnf::to_anf(ast);
  std::cout << ast->toString() << std::endl;
  std::cout << anf->toString() << std::endl;
  Instructions *ins = InstructionSelector::to_select(anf);
  std::cout << "hello" << std::endl;
  std::string program = ins->to_assembly();
  std::cout << "hello" << std::endl;
  std::cout << program << std::endl;
}
