#include <algorithm>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

class Expression {
public:
  virtual ~Expression() = default;
  virtual string toString() const = 0;
};

class NumberExpression : public Expression {
  int value;

public:
  NumberExpression(int value) : value(value) {}
  ~NumberExpression() override = default;
  string toString() const override { return to_string(value); }
  int get_value() { return value; }
};

class VariableExpression : public Expression {
  string name;

public:
  VariableExpression(const string &name) : name(name) {}
  ~VariableExpression() override = default;
  string toString() const override { return name; }
  string get_name() { return name; }
};

class AdditionExpression : public Expression {
  Expression *left;
  Expression *right;

public:
  AdditionExpression(Expression *left, Expression *right)
      : left(left), right(right) {}
  ~AdditionExpression() override {
    delete left;
    delete right;
  }
  string toString() const override {
    return "(+ " + left->toString() + " " + right->toString() + ")";
  }

  Expression *get_left() { return left; }
  Expression *get_right() { return right; }
};

class LessExpression : public Expression {
  Expression *left;
  Expression *right;

public:
  LessExpression(Expression *left, Expression *right)
      : left(left), right(right) {}
  ~LessExpression() override {
    delete left;
    delete right;
  }
  string toString() const override {
    return "(< " + left->toString() + " " + right->toString() + ")";
  }

  Expression *get_left() { return left; }
  Expression *get_right() { return right; }
};

class LetExpression : public Expression {
  string variable;
  Expression *value;
  Expression *body;

public:
  LetExpression(const string &variable, Expression *value, Expression *body)
      : variable(variable), value(value), body(body) {}
  ~LetExpression() override {
    delete value;
    delete body;
  }
  string toString() const override {
    return "(let ((" + variable + " " + value->toString() + ")) " +
           body->toString() + ")";
  }

  string get_variable() { return variable; }
  Expression *get_value() { return value; }
  Expression *get_body() { return body; }
};

class SetExpression : public Expression {
  string variable;
  Expression *value;

public:
  SetExpression(const string &variable, Expression *value)
      : variable(variable), value(value) {}
  ~SetExpression() override { delete value; }
  string toString() const override {
    return "(set " + variable + " " + value->toString() + ")";
  }
  Expression *get_value() { return value; }
  string get_variable() { return variable; }
};

class IfExpression : public Expression {
  Expression *cnd;
  Expression *thn;
  Expression *els;

public:
  IfExpression(Expression *cnd, Expression *thn, Expression *els)
      : cnd(cnd), thn(thn), els(els) {}
  ~IfExpression() override {
    delete cnd;
    delete thn;
    delete els;
  }
  string toString() const override {
    return "(if " + cnd->toString() + " " + thn->toString() + " " +
           els->toString() + ")";
  }

  Expression *get_cnd() { return cnd; }
  Expression *get_thn() { return thn; }
  Expression *get_els() { return els; }
};

class WhileExpression : public Expression {
  Expression *cnd;
  Expression *body;

public:
  WhileExpression(Expression *cnd, Expression *body) : cnd(cnd), body(body) {}
  ~WhileExpression() override {
    delete cnd;
    delete body;
  }
  string toString() const override {
    return "(while " + cnd->toString() + " " + body->toString() + ")";
  }

  Expression *get_cnd() { return cnd; }
  Expression *get_body() { return body; }
};

class BeginExpression : public Expression {
  vector<Expression *> *expressions;

public:
  BeginExpression(vector<Expression *> *expressions)
      : expressions(expressions) {}
  BeginExpression() : expressions(new vector<Expression *>{}) {}

  ~BeginExpression() override {
    for (Expression *expr : *expressions) {
      delete expr;
    }
    delete expressions;
  }

  void push_exp(Expression *exp) { expressions->push_back(exp); }

  string toString() const override {
    string result = "(begin";
    for (const auto &expr : *expressions) {
      result += " " + expr->toString();
    }
    result += ")";
    return result;
  }

  vector<Expression *> *get_expressions() { return expressions; }
};

class Parser {
public:
  static Expression *parse(const string &program) {
    vector<string> tokens = tokenize(program);
    size_t index = 0;
    return parseExpression(tokens, index);
  }

private:
  static vector<string> tokenize(const string &program) {
    vector<string> tokens;
    string currentToken;
    for (char ch : program) {
      switch (ch) {
      case '(':
      case ')':
      case ' ':
        if (!currentToken.empty()) {
          tokens.push_back(currentToken);
          currentToken.clear();
        }
        if (ch != ' ')
          tokens.push_back(string(1, ch));
        break;
      default:
        currentToken += ch;
        break;
      }
    }
    if (!currentToken.empty()) {
      tokens.push_back(currentToken);
    }
    return tokens;
  }

  static Expression *parseExpression(const vector<string> &tokens,
                                     size_t &index) {
    const string &token = tokens[index];

    if (token == "(") {
      ++index;
      const string &nextToken = tokens[index];

      if (nextToken == "+") {
        ++index;
        Expression *left = parseExpression(tokens, index);
        ++index;
        Expression *right = parseExpression(tokens, index);
        ++index; // Skip closing ')'
        return new AdditionExpression(left, right);
      } else if (nextToken == "<") {
        ++index;
        Expression *left = parseExpression(tokens, index);
        ++index;
        Expression *right = parseExpression(tokens, index);
        ++index; // Skip closing ')'
        return new LessExpression(left, right);
      } else if (nextToken == "if") {
        ++index; // Skip "if"
        Expression *cnd = parseExpression(tokens, index);
        ++index;
        Expression *thn = parseExpression(tokens, index);
        ++index;
        Expression *els = parseExpression(tokens, index);
        ++index; // Skip closing ')'
        return new IfExpression(cnd, thn, els);
      } else if (nextToken == "begin") {
        ++index;
        BeginExpression *bgn = new BeginExpression();
        while (tokens[index] != ")") {
          Expression *exp = parseExpression(tokens, index);
          bgn->push_exp(exp);
          ++index;
        }
        ++index; // Skip closing ')'
        return bgn;
      } else if (nextToken == "set") {
        ++index;
        string variable = tokens[index++];
        Expression *value = parseExpression(tokens, index);
        ++index;
        return new SetExpression(variable, value);
      } else if (nextToken == "let") {
        ++index; // Skip "let"
        ++index; // Skip first "("
        ++index;
        string variable = tokens[index++];
        Expression *value = parseExpression(tokens, index);
        ++index; // Skip ")"
        ++index;
        ++index;
        Expression *body = parseExpression(tokens, index);
        ++index; // Skip closing ')'
        return new LetExpression(variable, value, body);
      } else if (nextToken == "while") {
        ++index;
        Expression *cnd = parseExpression(tokens, index);
        ++index;
        Expression *body = parseExpression(tokens, index);
        ++index; // Skip closing ')'
        return new WhileExpression(cnd, body);
      }
    } else if (isNumber(token)) {
      return new NumberExpression(stoi(token));
    } else {
      return new VariableExpression(token);
    }

    return nullptr;
  }

  static bool isNumber(const string &token) {
    return !token.empty() && all_of(token.begin(), token.end(), ::isdigit);
  }
};
/*
int main() {
  string program = "(let ((sum 0)) (let ((i 0)) (begin (while (< i 5) (begin
(set sum (+ sum 2)) (set i (+ i 1)))) sum)))"; Expression *expression =
Parser::parse(program); if (expression) { cout << expression->toString() <<
endl; delete expression; } else { cout << "Parsing failed." << endl;
  }
  return 0;
}
*/
