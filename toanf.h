#ifndef TOANF_H
#define TOANF_H

#include "parser.h" 
#include <iostream>
#include <memory>
#include <string>
#include <vector>

class ToAnf {
public:
    static Expression* to_anf(Expression* ast);

private:
    static Expression* ast_to_anf(Expression* ast, size_t& counter);
};

#endif // TOANF_H
