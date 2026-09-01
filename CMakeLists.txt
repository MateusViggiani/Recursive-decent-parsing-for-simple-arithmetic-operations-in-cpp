cmake_minimum_required(VERSION 3.20)

if(NOT CMAKE_CXX_COMPILER)
  set(CMAKE_CXX_COMPILER clang++)
endif()

project(Parser LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic -Werror)

add_library(core
  src/lexer.cpp
)
target_include_directories(core PUBLIC include)

option(ENABLE_SANITIZERS "AddressSanitizer + UBSan" OFF)
if(ENABLE_SANITIZERS)
  target_compile_options(core PRIVATE -fsanitize=address,undefined -g)
  target_link_options(core PRIVATE -fsanitize=address,undefined)
endif()
