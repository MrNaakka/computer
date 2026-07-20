#include "encode.hpp"
#include "tokenize.hpp"
#include "asm-error.hpp"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc != 3) {
    std::cerr << "usage: assembler <in.asm> <out.b>" << std::endl;
    return 1;
  }

  std::ifstream in(argv[1]);
  if (!in.is_open()) {
    std::cerr << "cannot open " << argv[1] << std::endl;
    return 1;
  }

  try {
    auto lines = tokenize(in);
    auto symbolTable = buildSymbolTable(lines);
    auto instructions = encode(lines, symbolTable);

    std::ofstream o(argv[2]);
    for (auto instruction : instructions) {
      o << std::bitset<32>(instruction) << std::endl;
    }
  } catch (AsmError &e) {
    std::cout << e.line << ": " << e.message << std::endl;
  }

  return 0;
}
