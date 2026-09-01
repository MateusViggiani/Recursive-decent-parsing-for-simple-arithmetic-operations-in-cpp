#pragma once

#include "token.hpp"
#include <cstddef>
#include <vector>

class parser {
private:
  std::vector<token> tokens;

  std::size_t pos;

  double expressao();

  double termo();

  double fator();

  const token &espiar() const;

  token consumir();

  bool acabou() const;

public:
  parser(const std::vector<token> &tokens);
  double avaliar();
};
