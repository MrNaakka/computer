#include "../alu.hpp"
#include "../gates.hpp"
#include "_test.hpp"
#include <array>

Gate in1{true};
Gate in2{true};

Gate inv1{true};
Gate inv2{true};

Gate noCarryIn{false};

std::array<Gate, 2> andOP{Gate{false}, {false}};
std::array<Gate, 2> orOP{Gate{true}, {false}};
std::array<Gate, 2> addOP{Gate{false}, {true}};

bool aluAnd1() {
  return alu1Bit(in1, in2, !inv1, !inv2, noCarryIn, andOP).result.value == true;
}
bool aluAnd2() {
  return alu1Bit(in1, !in2, !inv1, !inv2, noCarryIn, andOP).result.value ==
         false;
}
bool aluAnd3() {
  return alu1Bit(!in1, in2, !inv1, !inv2, noCarryIn, andOP).result.value ==
         false;
}

bool aluOr1() {
  return alu1Bit(in1, in2, !inv1, !inv2, noCarryIn, orOP).result.value == true;
}
bool aluOr2() {
  return alu1Bit(!in1, in2, !inv1, !inv2, noCarryIn, orOP).result.value == true;
}
bool aluOr3() {
  return alu1Bit(!in1, !in2, !inv1, !inv2, noCarryIn, orOP).result.value ==
         false;
}

bool aluAdd1() {
  auto res = alu1Bit(in1, in2, !inv1, !inv2, !noCarryIn, addOP);
  return res.result.value == true && res.carry_out.value == true;
}
bool aluAdd2() {
  auto res = alu1Bit(in1, in2, !inv1, !inv2, noCarryIn, addOP);
  return res.result.value == false && res.carry_out.value == true;
}

bool aluAdd3() {
  auto res = alu1Bit(in1, !in2, !inv1, !inv2, noCarryIn, addOP);
  return res.result.value == true && res.carry_out.value == false;
}
bool aluAdd4() {
  auto res = alu1Bit(!in1, !in2, !inv1, !inv2, noCarryIn, addOP);
  return res.result.value == false && res.carry_out.value == false;
}

bool aluAdd6() {
  auto res = alu1Bit(in1, !in2, !inv1, inv2, !noCarryIn, addOP);
  return res.result.value == true && res.carry_out.value == true;
}



bool busAdd1() {
  Bus in1 {};
  Bus in2 {};
  in1[1] = {true};

  in2[2] = {true};
  in2[1] = {true};

  auto res = aluBus(in1, in2, !inv1, !inv2, addOP);
  Bus resBus {};
  resBus[3] = {true};
  auto re = (res.result == resBus).value == true;
  return re;
}

void ALUTESTS() {
  std::cout << "\n Now running alu tests \n";
  CHECK(aluAnd1(), "and1");
  CHECK(aluAnd2(), "and2");
  CHECK(aluAnd3(), "and3");

  CHECK(aluOr1(), "or1");
  CHECK(aluOr2(), "or2");
  CHECK(aluOr3(), "or3");

  CHECK(aluAdd1(), "add1");
  CHECK(aluAdd2(), "add2");
  CHECK(aluAdd3(), "add3");
  CHECK(aluAdd4(), "add4");
  CHECK(aluAdd6(), "add6");

  CHECK(busAdd1(), "busAdd1");

  std::cout << "\n Alu testes finnished \n";
}
