#include "../combinational/alu.hpp"
#include "../combinational/gates.hpp"
#include "_test.hpp"
#include <array>

void aluAnd() {
  Gate in1{true};
  Gate in2{true};

  Gate inv1{true};
  Gate inv2{true};
  Gate less{false};

  Gate noCarryIn{false};
  std::array<Gate, 2> andOP{Gate{false}, {false}};
  CHECK(alu1Bit(in1, in2, less, !inv1, !inv2, noCarryIn, andOP).result.value ==
        true);
  CHECK(alu1Bit(in1, !in2, less, !inv1, !inv2, noCarryIn, andOP).result.value ==
        false);
  CHECK(alu1Bit(!in1, in2, less, !inv1, !inv2, noCarryIn, andOP).result.value ==
        false);
}

void aluOr() {
  Gate in1{true};
  Gate in2{true};
  Gate less{false};

  Gate inv1{true};
  Gate inv2{true};

  Gate noCarryIn{false};
  std::array<Gate, 2> orOP{Gate{true}, {false}};
  CHECK(alu1Bit(in1, in2, less, !inv1, !inv2, noCarryIn, orOP).result.value ==
        true);
  CHECK(alu1Bit(!in1, in2, less, !inv1, !inv2, noCarryIn, orOP).result.value ==
        true);
  CHECK(alu1Bit(!in1, !in2, less, !inv1, !inv2, noCarryIn, orOP).result.value ==
        false);
}

void aluAdd() {
  Gate in1{true};
  Gate in2{true};
  Gate less{false};

  Gate inv1{true};
  Gate inv2{true};

  Gate noCarryIn{false};

  std::array<Gate, 2> addOP{Gate{false}, {true}};
  auto res1 = alu1Bit(in1, in2, less, !inv1, !inv2, !noCarryIn, addOP);
  CHECK(res1.result.value == true && res1.carry_out.value == true);

  auto res2 = alu1Bit(in1, in2, less, !inv1, !inv2, noCarryIn, addOP);
  CHECK(res2.result.value == false && res2.carry_out.value == true);

  auto res3 = alu1Bit(in1, !in2, less, !inv1, !inv2, noCarryIn, addOP);
  CHECK(res3.result.value == true && res3.carry_out.value == false);

  auto res4 = alu1Bit(!in1, !in2, less, !inv1, !inv2, noCarryIn, addOP);
  CHECK(res4.result.value == false && res4.carry_out.value == false);

  auto res6 = alu1Bit(in1, !in2, less, !inv1, inv2, !noCarryIn, addOP);
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

  std::array<Gate, 2> addOP{Gate{false}, {true}};
  auto res = aluBus(in1, in2, !inv1, !inv2, addOP);
  std::cout << "nyt: " << res.result << std::endl;
  Bus resBus{};
  resBus[3] = {true};

  std::cout << "nyt resBus: " << resBus << std::endl;
  CHECK((res.result == resBus).value == true);
}

void busCompare() {

  Gate inv1{true};
  Gate inv2{true};

  Bus in1{};
  Bus in2{};
  in1[1] = {true};

  in2[2] = {true};
  in2[1] = {true};


  std::array<Gate, 2> lessThanOP{Gate{true}, {true}};

  auto res = aluBus(in1, in2, !inv1, inv2, lessThanOP);
  Bus checkBus {};
  checkBus[0] = {true};

  CHECK((checkBus == res.result).value == true);
  res = aluBus(in2, in1, !inv1, inv2, lessThanOP);

  CHECK((checkBus == res.result).value == false);
}

void ALUTESTS() {
  std::cout << "\n Now running alu tests \n";
  aluAnd();
  aluOr();

  aluAdd();

  busAdd();

  busCompare();
  std::cout << "\n Alu testes finnished \n";
}
