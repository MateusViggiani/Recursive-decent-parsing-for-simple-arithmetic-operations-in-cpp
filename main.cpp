

#include "lexer.hpp"
#include "parser.hpp"
#include <exception>
#include <iostream>
#include <string>

int main() {

  std::string expression;

  std::cin >> expression;

  try {
    parser parse{tokenize(expression)};
    std::cout << parse.avaliar();
  } catch (const std::exception &e) {
    std::cerr << e.what();
  }

  return 0;
}
