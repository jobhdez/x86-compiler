#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class Expression;

class Instructions {
  std::vector<std::vector<std::string>> *instructions;

public:
  Instructions(std::vector<std::vector<std::string>> *instructions);
  std::vector<std::vector<std::string>> *get_instructions();
  std::string to_assembly();
};

class InstructionSelector {
public:
  static Instructions *to_select(Expression *anf);

private:
  static std::vector<std::vector<std::string>> *
  anf_to_select(Expression *anf, size_t &counter,
                std::unordered_map<std::string, std::string> &stack);
};

#endif // INSTRUCTIONS_H
