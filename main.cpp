#include "parser.h"
#include "toanf.h"
#include "toselect.h"

int main() {
  std::string input = "(let ((x 0)) (let ((y 8)) (if (< y 2) (if (< x 3) 1 2) 3)))";
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
