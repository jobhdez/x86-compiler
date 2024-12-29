#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H

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
    NumberExpression(int value);
    ~NumberExpression() override = default;
    string toString() const override;
    int get_value();
};

class VariableExpression : public Expression {
    string name;

public:
    VariableExpression(const string &name);
    ~VariableExpression() override = default;
    string toString() const override;
    string get_name();
};

class AdditionExpression : public Expression {
    Expression *left;
    Expression *right;

public:
    AdditionExpression(Expression *left, Expression *right);
    ~AdditionExpression() override;
    string toString() const override;
    Expression *get_left();
    Expression *get_right();
};

class LessExpression : public Expression {
    Expression *left;
    Expression *right;

public:
    LessExpression(Expression *left, Expression *right);
    ~LessExpression() override;
    string toString() const override;
    Expression *get_left();
    Expression *get_right();
};

class LetExpression : public Expression {
    string variable;
    Expression *value;
    Expression *body;

public:
    LetExpression(const string &variable, Expression *value, Expression *body);
    ~LetExpression() override;
    string toString() const override;
    string get_variable();
    Expression *get_value();
    Expression *get_body();
};

class SetExpression : public Expression {
    string variable;
    Expression *value;

public:
    SetExpression(const string &variable, Expression *value);
    ~SetExpression() override;
    string toString() const override;
    Expression *get_value();
    string get_variable();
};

class IfExpression : public Expression {
    Expression *cnd;
    Expression *thn;
    Expression *els;

public:
    IfExpression(Expression *cnd, Expression *thn, Expression *els);
    ~IfExpression() override;
    string toString() const override;
    Expression *get_cnd();
    Expression *get_thn();
    Expression *get_els();
};

class WhileExpression : public Expression {
    Expression *cnd;
    Expression *body;

public:
    WhileExpression(Expression *cnd, Expression *body);
    ~WhileExpression() override;
    string toString() const override;
    Expression *get_cnd();
    Expression *get_body();
};

class BeginExpression : public Expression {
    vector<Expression *> *expressions;

public:
    BeginExpression(vector<Expression *> *expressions);
    BeginExpression();
    ~BeginExpression() override;
    void push_exp(Expression *exp);
    string toString() const override;
    vector<Expression *> *get_expressions();
};

class Parser {
public:
    static Expression *parse(const string &program);

private:
    static vector<string> tokenize(const string &program);
    static Expression *parseExpression(const vector<string> &tokens, size_t &index);
    static bool isNumber(const string &token);
};

#endif // EXPRESSIONS_H
