#include "lexer.hpp"
#include "parser.hpp"
#include "token.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>

// helper: avalia uma expressao do inicio ao fim
double avaliar(const std::string &expressao) {
  parser p(tokenize(expressao));
  return p.avaliar();
}

// helper: true se a chamada lancou excecao
template <typename F> bool lancou(F f) {
  try {
    f();
    return false;
  } catch (...) {
    return true;
  }
}

// helper: comparacao de double com tolerancia
bool proximo(double a, double b) { return std::fabs(a - b) < 1e-9; }

// ==================== lexer ====================

void teste_lexer_conta_tokens() {
  assert(tokenize("3+4").size() == 3);
  assert(tokenize("12*5").size() == 3);
  assert(tokenize("(2-1)").size() == 5);
  assert(tokenize("42").size() == 1);
  std::cout << "  ok: quantidade de tokens\n";
}

void teste_lexer_numero_multiplos_digitos() {
  auto ts = tokenize("123");
  assert(ts.size() == 1);
  assert(std::holds_alternative<double>(ts[0]));
  assert(proximo(std::get<double>(ts[0]), 123));
  std::cout << "  ok: agrupa digitos num numero so\n";
}

void teste_lexer_decimais() {
  auto ponto = tokenize("3.5");
  assert(proximo(std::get<double>(ponto[0]), 3.5));
  auto virgula = tokenize("3,5");
  assert(proximo(std::get<double>(virgula[0]), 3.5));
  std::cout << "  ok: aceita ponto e virgula como decimal\n";
}

void teste_lexer_ignora_espacos() {
  assert(tokenize("3 + 4").size() == tokenize("3+4").size());
  assert(tokenize("  1  ").size() == 1);
  std::cout << "  ok: espacos sao ignorados\n";
}

void teste_lexer_string_vazia() {
  assert(tokenize("").empty());
  std::cout << "  ok: string vazia gera lista vazia\n";
}

void teste_lexer_caractere_invalido() {
  assert(lancou([] { tokenize("3 & 4"); }));
  assert(lancou([] { tokenize("abc"); }));
  std::cout << "  ok: rejeita caractere invalido\n";
}

void teste_lexer_numero_malformado() {
  assert(lancou([] { tokenize("3..5"); }));
  assert(lancou([] { tokenize("1.2.3"); }));
  std::cout << "  ok: rejeita numero com dois pontos\n";
}

// ==================== parser: basico ====================

void teste_numero_sozinho() {
  assert(proximo(avaliar("42"), 42));
  assert(proximo(avaliar("3.5"), 3.5));
  std::cout << "  ok: numero sozinho\n";
}

void teste_operacoes_simples() {
  assert(proximo(avaliar("2+3"), 5));
  assert(proximo(avaliar("10-4"), 6));
  assert(proximo(avaliar("6*7"), 42));
  assert(proximo(avaliar("8/2"), 4));
  std::cout << "  ok: as quatro operacoes\n";
}

// ==================== parser: precedencia ====================

void teste_precedencia_multiplicacao() {
  // o teste central: 11, nao 14
  assert(proximo(avaliar("3+4*2"), 11));
  assert(proximo(avaliar("2*3+4"), 10));
  std::cout << "  ok: multiplicacao antes da soma\n";
}

void teste_precedencia_divisao() {
  assert(proximo(avaliar("10-6/2"), 7));
  assert(proximo(avaliar("6/2+1"), 4));
  std::cout << "  ok: divisao antes da subtracao\n";
}

// ==================== parser: associatividade ====================

void teste_associatividade_esquerda_subtracao() {
  // (10-3)-2 = 5, nao 10-(3-2) = 9
  assert(proximo(avaliar("10-3-2"), 5));
  std::cout << "  ok: subtracao associa a esquerda\n";
}

void teste_associatividade_esquerda_divisao() {
  // (16/4)/2 = 2, nao 16/(4/2) = 8
  assert(proximo(avaliar("16/4/2"), 2));
  std::cout << "  ok: divisao associa a esquerda\n";
}

void teste_varios_operadores_seguidos() {
  assert(proximo(avaliar("1+2+3+4"), 10));
  assert(proximo(avaliar("2*3*4"), 24));
  assert(proximo(avaliar("1+2*3-4/2"), 5));
  std::cout << "  ok: sequencias longas\n";
}

// ==================== parser: parenteses ====================

void teste_parenteses_mudam_precedencia() {
  assert(proximo(avaliar("(3+4)*2"), 14));
  assert(proximo(avaliar("3*(4+2)"), 18));
  std::cout << "  ok: parenteses sobrepoem a precedencia\n";
}

void teste_parenteses_aninhados() {
  assert(proximo(avaliar("((1+2))"), 3));
  assert(proximo(avaliar("((1+2)*3)"), 9));
  assert(proximo(avaliar("2*((3+1)*(2+2))"), 32));
  std::cout << "  ok: parenteses aninhados\n";
}

void teste_parenteses_redundantes() {
  assert(proximo(avaliar("(42)"), 42));
  assert(proximo(avaliar("(((7)))"), 7));
  std::cout << "  ok: parenteses redundantes\n";
}

void teste_operacao_completa_dentro_do_parenteses() {
  // prova que fator() chama expressao(), nao termo()
  assert(proximo(avaliar("(2*3+1)"), 7));
  assert(proximo(avaliar("(1+2*3)"), 7));
  std::cout << "  ok: precedencia funciona dentro do parenteses\n";
}

// ==================== parser: erros ====================

void teste_divisao_por_zero() {
  assert(lancou([] { avaliar("5/0"); }));
  assert(lancou([] { avaliar("1+8/0"); }));
  std::cout << "  ok: rejeita divisao por zero\n";
}

void teste_zero_dividido_e_valido() {
  // 0 como DIVIDENDO e valido, so o divisor nao pode ser 0
  assert(proximo(avaliar("0/5"), 0));
  std::cout << "  ok: 0/5 e valido\n";
}

void teste_expressao_incompleta() {
  assert(lancou([] { avaliar("3+"); }));
  assert(lancou([] { avaliar("*5"); }));
  assert(lancou([] { avaliar(""); }));
  std::cout << "  ok: rejeita expressao incompleta\n";
}

void teste_parenteses_desbalanceados() {
  assert(lancou([] { avaliar("(3+4"); }));
  assert(lancou([] { avaliar("3+4)"); }));
  assert(lancou([] { avaliar("((1+2)"); }));
  std::cout << "  ok: rejeita parenteses desbalanceados\n";
}

void teste_lixo_no_final() {
  // expressao valida seguida de token nao consumido
  assert(lancou([] { avaliar("3 4"); }));
  assert(lancou([] { avaliar("(1+2))"); }));
  std::cout << "  ok: rejeita token sobrando no final\n";
}

void teste_parenteses_vazios() {
  assert(lancou([] { avaliar("()"); }));
  std::cout << "  ok: rejeita parenteses vazios\n";
}

int main() {
  std::cout << "lexer:\n";
  teste_lexer_conta_tokens();
  teste_lexer_numero_multiplos_digitos();
  teste_lexer_decimais();
  teste_lexer_ignora_espacos();
  teste_lexer_string_vazia();
  teste_lexer_caractere_invalido();
  teste_lexer_numero_malformado();

  std::cout << "parser (basico):\n";
  teste_numero_sozinho();
  teste_operacoes_simples();

  std::cout << "parser (precedencia):\n";
  teste_precedencia_multiplicacao();
  teste_precedencia_divisao();

  std::cout << "parser (associatividade):\n";
  teste_associatividade_esquerda_subtracao();
  teste_associatividade_esquerda_divisao();
  teste_varios_operadores_seguidos();

  std::cout << "parser (parenteses):\n";
  teste_parenteses_mudam_precedencia();
  teste_parenteses_aninhados();
  teste_parenteses_redundantes();
  teste_operacao_completa_dentro_do_parenteses();

  std::cout << "parser (erros):\n";
  teste_divisao_por_zero();
  teste_zero_dividido_e_valido();
  teste_expressao_incompleta();
  teste_parenteses_desbalanceados();
  teste_lixo_no_final();
  teste_parenteses_vazios();

  std::cout << "\nTODOS OS TESTES PASSARAM\n";
  return 0;
}
