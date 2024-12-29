#include "toanf.cpp"
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class Instructions {
  vector<vector<string>> *instructions;

public:
  Instructions(vector<vector<string>> *instructions)
      : instructions(instructions) {}

  vector<vector<string>> *get_instructions() { return instructions; }

  string to_assembly() {
    string program;
    for (auto &instruction : *instructions) {

      for (const auto &line : instruction) {
        program += line;
      }
    }
    return program;
  }
};

class InstructionSelector {
public:
  static Instructions *to_select(Expression *anf) {
    size_t counter = 0;
    unordered_map<string, string> stack;
    auto *main_instructions = anf_to_select(anf, counter, stack);
    size_t stack_;
    if ((counter % 16) == 0) {
      stack_ = counter;
    } else {
      stack_ = counter += 8;
    }
    vector<vector<string>> *prelude_instructions = new vector<vector<string>>();
    vector<string> in_ = {"\t.globl ", "main\n"};
    vector<string> in2_ = {"main:\n"};
    vector<string> in3_ = {"\tpushq ", "%rbp\n"};
    vector<string> in4_ = {"\tmovq ", "%rsp, ", "%rbp\n"};
    vector<string> in5_ = {"\tsubq ", "$" + to_string(stack_) + ", ", "%rsp\n"};

    prelude_instructions->push_back(in_);
    prelude_instructions->push_back(in2_);
    prelude_instructions->push_back(in3_);
    prelude_instructions->push_back(in4_);
    prelude_instructions->push_back(in5_);

    vector<vector<string>> *conclusion_instructions =
        new vector<vector<string>>();
    vector<string> in__ = {"\nconclusion:\n\n"};
    vector<string> in2__ = {"\taddq ", "$" + to_string(stack_) + ", ",
                            "%rsp\n"};
    vector<string> in3__ = {"\tpopq ", "%rbp\n"};
    vector<string> in4__ = {"\tretq"};
    conclusion_instructions->push_back(in__);
    conclusion_instructions->push_back(in2__);
    conclusion_instructions->push_back(in3__);
    conclusion_instructions->push_back(in4__);

    prelude_instructions->insert(prelude_instructions->end(),
                                 main_instructions->begin(),
                                 main_instructions->end());
    prelude_instructions->insert(prelude_instructions->end(),
                                 conclusion_instructions->begin(),
                                 conclusion_instructions->end());
    return new Instructions(prelude_instructions);
  }

private:
  static vector<vector<string>> *
  anf_to_select(Expression *anf, size_t &counter,
                unordered_map<string, string> &stack) {

    vector<vector<string>> instructions;

    if (LetExpression *anf_let = dynamic_cast<LetExpression *>(anf)) {
      string let_var = anf_let->get_variable();

      if (stack.find(let_var) == stack.end()) {
        counter += 8;
        string number_string = to_string(counter);
        string stack_location = "-" + number_string + "(%rbp)";
        stack[let_var] = stack_location;
        Expression *value = anf_let->get_value();

        if (NumberExpression *number_expr =
                dynamic_cast<NumberExpression *>(value)) {
          int val = number_expr->get_value();
          string immediate = "$" + to_string(val);
          vector<string> mov = {"\tmovq ", immediate + ", ",
                                stack_location + "\n"};

          instructions.push_back(mov);

        } else if (LessExpression *less_expr =
                       dynamic_cast<LessExpression *>(value)) {
          Expression *left = less_expr->get_left();
          VariableExpression *var_expr =
              dynamic_cast<VariableExpression *>(left);
          Expression *right = less_expr->get_right();
          NumberExpression *num_expr = dynamic_cast<NumberExpression *>(right);
          string var = var_expr->get_name();

          if (stack.find(var) != stack.end()) {
            string stack_location_ = stack[var];
            string immediate_ = to_string(num_expr->get_value());
            vector<string> cmp = {"\tcmpq ", immediate_ + ", ",
                                  stack_location_ + "\n"};
            vector<string> setl = {"\tsetl ", "%al\n"};
            vector<string> movz = {"\tmovzbq ", "%al, ", "%rsi\n"};
            instructions.push_back(cmp);
            instructions.push_back(setl);
            instructions.push_back(movz);
          }
        }
        auto body_instructions =
            anf_to_select(anf_let->get_body(), counter, stack);
        instructions.insert(instructions.end(), body_instructions->begin(),
                            body_instructions->end());

      } else {
        string stack_location = stack[let_var];
        Expression *value = anf_let->get_value();
        auto *number_expr = dynamic_cast<NumberExpression *>(value);
        int val = number_expr->get_value();
        string immediate = "$" + to_string(val);

        vector<string> mov = {"\tmovq ", immediate, stack_location + "\n"};
        instructions.push_back(mov);

        auto body_instructions =
            anf_to_select(anf_let->get_body(), counter, stack);
        instructions.insert(instructions.end(), body_instructions->begin(),
                            body_instructions->end());
      }

    } else if (IfExpression *if_expr = dynamic_cast<IfExpression *>(anf)) {
      Expression *cnd = if_expr->get_cnd();
      VariableExpression *var_cnd = dynamic_cast<VariableExpression *>(cnd);
      string stack_location__ = stack[var_cnd->get_name()];

      vector<string> cmp = {"\tcmpq ", "$1, ", "%rsi\n"};
      string block = "block_" + to_string(counter);
      size_t temp_counter = counter;
      ++temp_counter;
      string block2 = "block_" + to_string(temp_counter);
      instructions.push_back(cmp);
      vector<string> je = {"\tje ", block + "\n"};
      vector<string> jmp = {"\tjmp ", block2 + "\n"};
      instructions.push_back(je);
      instructions.push_back(jmp);
      vector<string> label = {"\n" + block + ":\n\n"};
      ;
      instructions.push_back(label);
      auto thn_instructions = anf_to_select(if_expr->get_thn(), counter, stack);
      instructions.insert(instructions.end(), thn_instructions->begin(),
                          thn_instructions->end());
      vector<string> conclusion = {"\tjmp ", "conclusion\n"};
      instructions.push_back(label);
      vector<string> label2 = {"\n" + block2 + ":\n\n"};
      instructions.push_back(label2);
      auto els_instructions = anf_to_select(if_expr->get_els(), counter, stack);
      instructions.insert(instructions.end(), els_instructions->begin(),
                          els_instructions->end());
      instructions.push_back(conclusion);

    } else if (NumberExpression *num_expr =
                   dynamic_cast<NumberExpression *>(anf)) {
      int val = num_expr->get_value();
      string immediate = "$" + to_string(val);
      vector<string> mov = {"\tmovq ", immediate + ", ", "%rdi\n"};
      instructions.push_back(mov);
      vector<string> print = {"\tcallq ", "print_int\n"};
      instructions.push_back(print);

    } else if (SetExpression *set_expr = dynamic_cast<SetExpression *>(anf)) {
      string var = set_expr->get_variable();
      Expression *exp = set_expr->get_value();
      if (NumberExpression *num_expr = dynamic_cast<NumberExpression *>(exp)) {
        string val = "$" + to_string(num_expr->get_value());
        vector<string> mv = {"\tmovq ", val, stack[var] + "\n"};
        instructions.push_back(mv);
      } else if (AdditionExpression *sum =
                     dynamic_cast<AdditionExpression *>(exp)) {
        Expression *left_exp = sum->get_left();
        Expression *right_exp = sum->get_right();

        if (VariableExpression *l_var =
                dynamic_cast<VariableExpression *>(left_exp)) {
          if (NumberExpression *r_num =
                  dynamic_cast<NumberExpression *>(right_exp)) {
            vector<string> mov_ = {"\tmovq ",
                                   "$" + to_string(r_num->get_value()) + ", ",
                                   "%rax\n"};
            vector<string> add_ = {"\taddq ", "%rax, ", stack[var] + "\n"};
            instructions.push_back(mov_);
            instructions.push_back(add_);
          } else if (VariableExpression *r_var =
                         dynamic_cast<VariableExpression *>(right_exp)) {
            vector<string> mov = {"\tmovq", stack[r_var->get_name()], "%rax\n"};
            vector<string> add = {"\taddq ", "%rax, ", stack[var] + "\n"};
            instructions.push_back(mov);
            instructions.push_back(add);
          }
        }
      }
    } else if (BeginExpression *bgn_exps =
                   dynamic_cast<BeginExpression *>(anf)) {
      auto exps = bgn_exps->get_expressions();
      for (int i = 0; i < exps->size(); i++) {
        auto in = anf_to_select(exps->at(i), counter, stack);
        for (int j = 0; j < in->size(); j++) {
          instructions.push_back(in->at(i));
        }
      }
    } else if (WhileExpression *while_expr =
                   dynamic_cast<WhileExpression *>(anf)) {
      Expression *cnd = while_expr->get_cnd();
      if (LessExpression *cnd_expr = dynamic_cast<LessExpression *>(cnd)) {
        Expression *left = cnd_expr->get_left();
        VariableExpression *var_e = dynamic_cast<VariableExpression *>(left);
        Expression *right = cnd_expr->get_right();
        NumberExpression *num_e = dynamic_cast<NumberExpression *>(right);

        vector<string> cmp = {"\tcmpq ",
                              "$" + to_string(num_e->get_value()) + ", ",
                              stack[var_e->get_name()] + "\n"};
        vector<string> jl = {"\tjl ", "loop_" + to_string(counter) + "\n"};

        Expression *body = while_expr->get_body();
        auto body_instructions = anf_to_select(body, counter, stack);

        vector<string> loop = {"\nloop_" + to_string(counter) + ":\n"};
        instructions.push_back(loop);
        instructions.insert(instructions.end(), body_instructions->begin(),
                            body_instructions->end());
        instructions.push_back(cmp);
        instructions.push_back(jl);
      }

    } else if (VariableExpression *var_expr =
                   dynamic_cast<VariableExpression *>(anf)) {
      string var = var_expr->get_name();
      string stack_location = stack[var];
      vector<string> movq = {"\tmovq ", stack_location, " %rdi\n"};
      instructions.push_back(movq);

      vector<string> print = {"\tcallq ", "print_int\n"};
      instructions.push_back(print);
    }

    vector<vector<string>> *instructs =
        new vector<vector<string>>(instructions);

    return instructs;
  }
};

int main() {
  string input = "(let ((x 0)) (let ((y 8)) (if (< y 2) (if (< x 3) 1 2) 3)))";
  //  string input = "(let ((x 3)) (begin (while (< x 5) (set x (+ x 1))) x))";
  // string input = "(let ((x 3)) (set x (+ x 1)))";
  /// string input = "(let ((x 3)) (begin x x))";
  //  string input = "(let ((sum 0)) (let ((i 0)) (begin (while (< i 5)
  //  (begin(set sum (+ sum 3)) (set i (+ i 1)))) sum)))";
  Expression *ast = Parser::parse(input);
  Expression *anf = ToAnf::to_anf(ast);
  cout << ast->toString() << endl;
  cout << anf->toString() << endl;
  Instructions *ins = InstructionSelector::to_select(anf);
  cout << "hello" << endl;
  string program = ins->to_assembly();
  cout << "hello" << endl;
  cout << program << endl;
}
