#include "parser.cpp"
#include <iostream>
#include <memory>
using namespace std;

class ToAnf {
public:
  static Expression *to_anf(Expression *ast) {
    size_t counter = 0;
    return ast_to_anf(ast, counter);
  }

private:
  static Expression *ast_to_anf(Expression *ast, size_t &counter) {
    if (!ast)
      return nullptr;

    if (IfExpression *ast_if = dynamic_cast<IfExpression *>(ast)) {
      string tmp = "temp_" + to_string(counter);
      VariableExpression *temp_var = new VariableExpression(tmp);
      ++counter;
      Expression *thn = ast_to_anf(ast_if->get_thn(), counter);
      Expression *els = ast_to_anf(ast_if->get_els(), counter);
      IfExpression *if_exp = new IfExpression(temp_var, thn, els);
      return new LetExpression(temp_var->get_name(), ast_if->get_cnd(), if_exp);
    } else if (LetExpression *ast_let = dynamic_cast<LetExpression *>(ast)) {
      Expression *body = ast_to_anf(ast_let->get_body(), counter);
      return new LetExpression(ast_let->get_variable(), ast_let->get_value(),
                               body);
    } else if (BeginExpression *ast_begin =
                   dynamic_cast<BeginExpression *>(ast)) {

      vector<Expression *> *expressions = new vector<Expression *>();
      vector<Expression *> *ast_expressions = ast_begin->get_expressions();

      if (ast_expressions) {
        for (size_t i = 0; i < ast_expressions->size(); i++) {
          Expression *exp = ast_expressions->at(i);
          if (exp) {
            Expression *anf_exp = ast_to_anf(exp, counter);
            if (anf_exp) {
              expressions->push_back(anf_exp);
            }
          }
        }
      }
      return new BeginExpression(expressions);
    } else if (SetExpression *ast_set = dynamic_cast<SetExpression *>(ast)) {
      Expression *val = ast_to_anf(ast_set->get_value(), counter);
      string var = ast_set->get_variable();
      return new SetExpression(var, val);
    } else if (WhileExpression *ast_while =
                   dynamic_cast<WhileExpression *>(ast)) {
      Expression *cnd = ast_to_anf(ast_while->get_cnd(), counter);
      Expression *body = ast_to_anf(ast_while->get_body(), counter);
      return new WhileExpression(cnd, body);
    }
    return ast;
  }
};
/*
int main() {
  //string input = "(let ((x 1)) (while (< x 5) (begin (set x (+ x 1)) x)))";
  string input = "(let ((x 1)) (let ((y 3)) (if (< x 5) (if (< y 2) 2 3) 3)))";
  Expression *ast = Parser::parse(input);
  if (ast) {
    Expression *anf = ToAnf::to_anf(ast);
    if (anf) {
      cout << anf->toString() << endl;
    }
  }
  return 0;
}
*/
