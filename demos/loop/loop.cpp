#include "../../combinational/gates.hpp"
#include "../../helpers/converters.hpp"
#include "../../helpers/snapshot.hpp"
#include "../../sequintial/computer.hpp"
#include <iostream>

int main() {

  Computer computer;

  std::array<uint32_t, 13> instructions{
      0b00000000000000000000000000000000, 0b00000001000000000000000000000010,
      0b00000010000000000000000000000000, 0b00000011000000000000000000001000,
      0b00000100000000000000000000000001, 0b00000110000000000000000000000111,
      0b00000111000000000000000000001100, 0b01010101001100100000000000000000,
      0b10110000000001010000000000001100, 0b00100000000000010000000000000000,
      0b00100010001001000000000000000000, 0b11000000000000000000000000000110,
      0b11110000000000000000000000000000

  };
  std::array<uint32_t, 13> addresses{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

  computer.bootLoader(intsToBusses(instructions), intsToBusses(addresses));
  while (!computer.readHalt().value) {
    computer.tick();
  }
  ComputerSnapshot cs =
      computerStateToComputerSnapshot(computer.getComputerState());
  std::cout << "sum result: " << cs.cpu.registers[0] << std::endl;

  std::cout << "lol" << std::endl;

  return 0;
}
