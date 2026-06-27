#include "../alu.hpp"
#include "../gates.hpp"
#include "_test.hpp"
#include <array>

std::array<Gate, 2> addOP{Gate{false}, {true}};

void aluAnd() {
  Gate in1{true};
  Gate in2{true};

  Gate inv1{true};
  Gate inv2{true};

  Gate noCarryIn{false};
  std::array<Gate, 2> andOP{Gate{false}, {false}};
  CHECK(alu1Bit(in1, in2, !inv1, !inv2, noCarryIn, andOP).result.value == true);
  CHECK(alu1Bit(in1, !in2, !inv1, !inv2, noCarryIn, andOP).result.value ==
        false);
  CHECK(alu1Bit(!in1, in2, !inv1, !inv2, noCarryIn, andOP).result.value ==
        false);
}

void aluOr() {
  Gate in1{true};
  Gate in2{true};

  Gate inv1{true};
  Gate inv2{true};

  Gate noCarryIn{false};
  std::array<Gate, 2> orOP{Gate{true}, {false}};
  CHECK(alu1Bit(in1, in2, !inv1, !inv2, noCarryIn, orOP).result.value == true);
  CHECK(alu1Bit(!in1, in2, !inv1, !inv2, noCarryIn, orOP).result.value == true);
  CHECK(alu1Bit(!in1, !in2, !inv1, !inv2, noCarryIn, orOP).result.value ==
        false);
}

void aluAdd() {
  Gate in1{true};
  Gate in2{true};

  Gate inv1{true};
  Gate inv2{true};

  Gate noCarryIn{false};

  auto res1 = alu1Bit(in1, in2, !inv1, !inv2, !noCarryIn, addOP);
  CHECK(res1.result.value == true && res1.carry_out.value == true);

  auto res2 = alu1Bit(in1, in2, !inv1, !inv2, noCarryIn, addOP);
  CHECK(res2.result.value == false && res2.carry_out.value == true);

  auto res3 = alu1Bit(in1, !in2, !inv1, !inv2, noCarryIn, addOP);
  CHECK(res3.result.value == true && res3.carry_out.value == false);

  auto res4 = alu1Bit(!in1, !in2, !inv1, !inv2, noCarryIn, addOP);
  CHECK(res4.result.value == false && res4.carry_out.value == false);

  auto res6 = alu1Bit(in1, !in2, !inv1, inv2, !noCarryIn, addOP);
  CHECK(res6.result.value == true && res6.carry_out.value == true);
}

void busAdd() {
  Gate inv1{true};
  Gate inv2{true};

  Bus in1{};
  Bus in2{};
  in1[1] = {true};

  in2[2] = {true};
  in2[1] = {true};

  auto res = aluBus(in1, in2, !inv1, !inv2, addOP);
  Bus resBus{};
  resBus[3] = {true};
  CHECK((res.result == resBus).value == false);
}

void ALUTESTS() {
  std::cout << "\n Now running alu tests \n";
  aluAnd();
  aluOr();

  aluAdd();

  busAdd();

  std::cout << "\n Alu testes finnished \n";
}
