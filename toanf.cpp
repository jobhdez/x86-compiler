#include "toanf.h"

Expression* ToAnf::to_anf(Expression* ast) {
    size_t counter = 0;
    return ast_to_anf(ast, counter);
}

Expression* ToAnf::ast_to_anf(Expression* ast, size_t& counter) {
    if (!ast)
        return nullptr;

    if (IfExpression* ast_if = dynamic_cast<IfExpression*>(ast)) {
        std::string tmp = "temp_" + std::to_string(counter);
        VariableExpression* temp_var = new VariableExpression(tmp);
        ++counter;
        Expression* thn = ast_to_anf(ast_if->get_thn(), counter);
        Expression* els = ast_to_anf(ast_if->get_els(), counter);
        IfExpression* if_exp = new IfExpression(temp_var, thn, els);
        return new LetExpression(temp_var->get_name(), ast_if->get_cnd(), if_exp);
    } else if (LetExpression* ast_let = dynamic_cast<LetExpression*>(ast)) {
        Expression* body = ast_to_anf(ast_let->get_body(), counter);
        return new LetExpression(ast_let->get_variable(), ast_let->get_value(), body);
    } else if (BeginExpression* ast_begin = dynamic_cast<BeginExpression*>(ast)) {
        std::vector<Expression*>* expressions = new std::vector<Expression*>();
        std::vector<Expression*>* ast_expressions = ast_begin->get_expressions();

        if (ast_expressions) {
            for (size_t i = 0; i < ast_expressions->size(); i++) {
                Expression* exp = ast_expressions->at(i);
                if (exp) {
                    Expression* anf_exp = ast_to_anf(exp, counter);
                    if (anf_exp) {
                        expressions->push_back(anf_exp);
                    }
                }
            }
        }
        return new BeginExpression(expressions);
    } else if (SetExpression* ast_set = dynamic_cast<SetExpression*>(ast)) {
        Expression* val = ast_to_anf(ast_set->get_value(), counter);
        std::string var = ast_set->get_variable();
        return new SetExpression(var, val);
    } else if (WhileExpression* ast_while = dynamic_cast<WhileExpression*>(ast)) {
        Expression* cnd = ast_to_anf(ast_while->get_cnd(), counter);
        Expression* body = ast_to_anf(ast_while->get_body(), counter);
        return new WhileExpression(cnd, body);
    }
    return ast;
}
