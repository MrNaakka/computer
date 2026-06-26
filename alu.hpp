#pragma once
#include "adder.hpp"
#include "gates.hpp"
#include "multiplexor.hpp"
#include <array>

inline WithCarrieOut<Gate> alu1Bit(Gate in1, Gate in2, Gate in1SS, Gate in2SS,
                                   Gate carryIn,
                                   std::array<Gate, 2> operatorSS) {

  Gate i1 = mux1Bit(in1, !in1, in1SS);
  Gate i2 = mux1Bit(in2, !in2, in2SS);

  Gate andRes = i1 & i2;
  Gate orRes = i1 | i2;

  auto addRes = add1Bit(i1, i2, carryIn);

  std::array<Gate, 3> muxIn{andRes, orRes, addRes.result};
  Gate result = mux1BitNto1(muxIn, operatorSS);
  return {.result = result, .carry_out = addRes.carry_out};
}

inline WithCarrieOut<Bus> aluBus(Bus &in1, Bus &in2, Gate in1SS, Gate in2SS,
                                 std::array<Gate, 2> operatorSS) {

  Gate carryIn = in2SS;
  Bus resultBus{};
  for (int i = 0; i < WORD; ++i) {
    auto res = alu1Bit(in1[i], in2[i], in1SS, in2SS, carryIn, operatorSS);
    resultBus[i] = res.result;
    carryIn = res.carry_out;
  }
  return {.result = resultBus, .carry_out = carryIn};
}
