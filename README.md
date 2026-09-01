# Avaliador de Expressões Aritméticas

Um avaliador de expressões matemáticas em C++ que respeita precedência de operadores e parênteses, implementado com **análise léxica** (lexer) e **recursive descent parsing**.

```
3 + 4 * 2       →  11
(3 + 4) * 2     →  14
2 * ((3+1) * 2) →  16
```

---

## Como funciona

O processamento acontece em duas etapas:

**1. Lexer** (`lexer.cpp`) — converte a string de entrada numa lista de *tokens*. A expressão `"12 + 5"` vira `[número 12] [operador +] [número 5]`. É aqui que dígitos consecutivos são agrupados num número só e espaços são descartados.

**2. Parser** (`parser.cpp`) — consome a lista de tokens e calcula o resultado, usando três funções mutuamente recursivas, uma por nível de precedência:

| Função | Trata | Chama |
|---|---|---|
| `expressao()` | `+` e `-` | `termo()` |
| `termo()` | `*` e `/` | `fator()` |
| `fator()` | números e `( ... )` | `expressao()` (dentro de parênteses) |

A precedência não vem de nenhuma tabela — ela **emerge** da ordem das chamadas. Como `expressao()` delega para `termo()` antes de somar, as multiplicações já estão resolvidas quando a soma acontece. É por isso que `3 + 4 * 2` dá 11, não 14.

Os parênteses funcionam porque `fator()` volta ao topo da cadeia: dentro de `( )`, todos os níveis de precedência ficam disponíveis novamente.

---

## Compilando

Requer CMake 3.20+ e um compilador com suporte a C++23 (clang++ por padrão).

```bash
cmake -B build
cmake --build build
```

Para trocar o compilador:

```bash
rm -rf build
cmake -B build -DCMAKE_CXX_COMPILER=g++
cmake --build build
```

Para compilar com AddressSanitizer e UBSan:

```bash
rm -rf build
cmake -B build -DENABLE_SANITIZERS=ON
cmake --build build
```

---

## Executando

```bash
./build/calc
```

Digite a expressão quando solicitado.

### Testes

```bash
./build/testes
```

A suíte cobre o lexer (agrupamento de dígitos, decimais, caracteres inválidos) e o parser (precedência, associatividade, parênteses aninhados, erros de sintaxe). Todos os testes usam `assert` — se algum falhar, o programa aborta indicando arquivo, linha e a expressão que falhou.

---

## Sintaxe suportada

| Elemento | Exemplos |
|---|---|
| Números inteiros | `42`, `7`, `1000` |
| Números decimais | `3.5`, `3,5` (ponto e vírgula são aceitos) |
| Operadores | `+`, `-`, `*`, `/` |
| Parênteses | `(`, `)` — podem ser aninhados |
| Espaços | ignorados: `3+4` e `3 + 4` são equivalentes |

### Exemplos válidos

```
42
3 + 4
3+4*2                 →  11   (multiplicação primeiro)
(3+4)*2               →  14   (parênteses sobrepõem)
10-3-2                →  5    (associativo à esquerda)
16/4/2                →  2
2*((3+1)*(2+2))       →  32
3,5 * 2               →  7
```

---

## Limitações conhecidas

### Não há operador unário de menos

O `-` é sempre interpretado como operação binária (subtração), nunca como sinal. Isso significa que **números negativos não podem ser escritos diretamente**:

```
5 * -3        ✗ erro de sintaxe
-5 + 3        ✗ erro de sintaxe
```

O contorno é escrever a negação como uma subtração a partir de zero:

```
5 * (0-3)     ✓  →  -15
(0-5) + 3     ✓  →  -2
```

**Por quê:** o lexer produz um token `operador::subtracao` para todo `-`, e o parser só sabe consumi-lo entre dois operandos. Quando `-3` aparece onde o parser espera um operando, `fator()` recebe um operador e lança erro.

**Como resolveria:** seria preciso tratar o menos unário em `fator()` — se o token atual for `subtracao`, consumi-lo, chamar `fator()` recursivamente e devolver o valor negado. O mesmo vale para um `+` unário. Isso exigiria também que o lexer distinguisse os dois casos, ou que o parser decidisse pelo contexto (um `-` é unário quando aparece no início da expressão ou logo após outro operador ou um `(` ).

### Outras limitações

- Não há operador de potência (`^`).
- Não há funções (`sqrt`, `sin`, etc.).
- Não há variáveis.
- Divisão por zero é rejeitada com exceção, em vez de produzir infinito.

---

## Erros tratados

O programa rejeita, com mensagem clara e sem crash:

| Entrada | Motivo |
|---|---|
| `3 & 4` | caractere inválido |
| `3..5` | número malformado |
| `3 +` | expressão incompleta |
| `(3+4` | parêntese não fechado |
| `3+4)` | parêntese sem abertura |
| `3 4` | token sobrando no final |
| `()` | parênteses vazios |
| `5/0` | divisão por zero |

---

## Estrutura do projeto

```
.
├── CMakeLists.txt
├── main.cpp                 programa principal
├── teste.cpp                suíte de testes
├── include/
│   ├── token.hpp            definição do tipo Token (std::variant)
│   ├── lexer.hpp            declaração do tokenize
│   └── parser.hpp           declaração da classe parser
└── src/
    ├── lexer.cpp            string → lista de tokens
    └── parser.cpp           lista de tokens → resultado
```

O tipo `token` é um `std::variant<double, operador, abreParen, fechaParen>` — uma união type-safe que guarda um dos quatro tipos e sabe qual está guardando. As structs vazias `abreParen` e `fechaParen` servem como marcadores distintos, já que não carregam valor.
