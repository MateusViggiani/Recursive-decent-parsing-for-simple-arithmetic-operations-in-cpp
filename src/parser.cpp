

#include "parser.hpp"
#include "token.hpp"
#include <stdexcept>
#include <variant>
#include <vector>

parser::parser(const std::vector<token> &tokens) : tokens{tokens}, pos{0} {}

bool parser::acabou() const { return this->pos >= tokens.size(); }

const token &parser::espiar() const {
  if (acabou()) {
    throw std::runtime_error("expressao incompleta");
  }
  return this->tokens[this->pos];
}

token parser::consumir() {
  if (acabou()) {
    throw std::runtime_error("expressao incompleta");
  }
  token tok = this->tokens[pos];
  pos++;
  return tok;
}

double parser::fator() {
  token tokenConsumida;
  double resultado;

  if (std::holds_alternative<double>(espiar())) {
    tokenConsumida = consumir();
    return std::get<double>(tokenConsumida);
  } else if (std::holds_alternative<abreParen>(espiar())) {
    consumir();
    resultado = expressao();
    if (!std::holds_alternative<fechaParen>(espiar())) {
      throw std::invalid_argument("sintaxe inválida");
    }
    consumir();
  } else {
    throw std::invalid_argument("sintaxe inválida");
  }

  return resultado;
}

double parser::termo() {

  double elemento = fator();

  while (!acabou() && std::holds_alternative<operador>(espiar()) &&
         (get<operador>(espiar()) == operador::multiplicacao ||
          get<operador>(espiar()) == operador::divisao)) {
    operador op = std::get<operador>(consumir());
    if (op == operador::multiplicacao) {
      elemento *= fator();
    } else {
      double temp = fator();
      if (temp == 0) {
        throw std::invalid_argument("Cannot divide by zero");
      }
      elemento /= temp;
    }
  }

  return elemento;
}
double parser::expressao() {

  double elemento = termo();

  while (!acabou() && std::holds_alternative<operador>(espiar()) &&
         (get<operador>(espiar()) == operador::soma ||
          get<operador>(espiar()) == operador::subtracao)) {
    operador op = std::get<operador>(consumir());
    if (op == operador::soma) {
      elemento += termo();
    } else {
      elemento -= termo();
    }
  }

  return elemento;
}

double parser::avaliar() {
  double elemento = expressao();
  if (!acabou()) {
    throw std::invalid_argument("Sintaxe inválida");
  }
  return elemento;
}
