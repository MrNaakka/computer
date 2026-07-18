

#include <array>
#include <cstdint>
#include <iostream>
#include <string>

#include "../combinational/adder.hpp"
#include "../combinational/alu.hpp"
#include "../combinational/gates.hpp"

void calculator() {
  uint32_t num1;
  uint32_t num2;
  std::string oper;
  bool play = true;
  while (play) {
    std::cout << "enter first num: ";
    std::cin >> num1;

    std::cout << "enter operator (+ or -): ";
    std::cin >> oper;

    std::cout << "enter second num: ";
    std::cin >> num2;

    Bus in1{};
    Bus in2{};

    for (int i = 0; i < WORD; i++) {
      in1[i] = {.value = static_cast<bool>((num1 >> i) & 1)};
      in2[i] = {.value = static_cast<bool>((num2 >> i) & 1)};
    }

    Gate isSubstrac{.value = false};
    if (oper == "-") {
      isSubstrac.value = true;
    }

    auto result = addBus(in1, in2, isSubstrac);
    std::cout << "here is the result num: " << result.result << std::endl;

    auto aluRes = aluBus(in1, in2, {.value = false}, isSubstrac, {false},
                         {Gate{.value = false}, Gate{.value = true}});
    std::cout << "here is the alu num: " << aluRes.result << std::endl;

    std::string cont;
    std::cout << "do you want to contineu? y/n: ";
    std::cin >> cont;

    if (cont == "n") {
      play = false;
    }
  }
}

int main() {
  calculator();
  return 0;
}
