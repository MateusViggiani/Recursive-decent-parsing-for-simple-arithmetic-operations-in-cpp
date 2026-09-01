
#include "lexer.hpp"
#include "token.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::size_t endNum(int pos, const std::string &expression) {
  for (std::size_t i = pos; i < expression.size(); i++) {
    if ((expression[i] != ',' && !isdigit(expression[i]) &&
         expression[i] != '.')) {
      return i;
    }
  }
  return expression.size();
}
bool isValidDouble(const std::string &subexpression) {
  if (subexpression.size() == 1) {
    return true;
  }
  int contaPontoOuVirgula = 0;
  for (auto a : subexpression) {
    if (a == '.' || a == ',')
      contaPontoOuVirgula++;
    if (contaPontoOuVirgula > 1)
      return false;
    if (!std::isdigit(a) && a != '.' && a != ',')
      return false;
  }
  return true;
}

double stringToDouble(const std::string &subexpression) {
  std::string normalized = subexpression;
  std::replace(normalized.begin(), normalized.end(), ',', '.');
  std::stringstream stream(normalized);
  double valor = 0;
  stream >> valor;
  return valor;
}

std::vector<token> tokenize(const std::string &expression) {
  std::vector<token> tokens;

  for (std::size_t i = 0; i < expression.size(); i++) {
    if (isdigit(expression[i])) {
      size_t j = endNum(i, expression);
      std::string subexpression = expression.substr(i, j - i);
      if (!isValidDouble(subexpression)) {
        throw std::invalid_argument("Invalid expression");
      } else {
        tokens.push_back(stringToDouble(subexpression));
      }

      i = j - 1;
      continue;
    }

    switch (expression[i]) {
    case '+':
      tokens.push_back(operador::soma);
      break;
    case '-':
      tokens.push_back(operador::subtracao);
      break;
    case '*':
      tokens.push_back(operador::multiplicacao);
      break;
    case '/':
      tokens.push_back(operador::divisao);
      break;
    case '(':
      tokens.push_back(abreParen{});
      break;
    case ')':
      tokens.push_back(fechaParen{});
      break;
    case ' ':
      break;
    default:
      throw std::invalid_argument("Invalid expression");
      break;
    }
  }

  return tokens;
}
