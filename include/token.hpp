#pragma once

#include <variant>

struct abreParen {};

struct fechaParen {};

enum class operador { soma, subtracao, multiplicacao, divisao };

using token = std::variant<double, operador, abreParen, fechaParen>;
