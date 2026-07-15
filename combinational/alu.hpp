#pragma once
#include "adder.hpp"
#include "gates.hpp"
#include "multiplexor.hpp"
#include <array>

struct Alu1BitResult {
  Gate result;
  Gate carry_out;
  Gate addOut;
};

inline Alu1BitResult alu1Bit(Gate in1, Gate in2, Gate less, Gate in1SS,
                             Gate in2SS, Gate carryIn,
                             std::array<Gate, 2> operatorSS) {

  Gate i1 = mux1Bit(in1, !in1, in1SS);
  Gate i2 = mux1Bit(in2, !in2, in2SS);

  Gate andRes = i1 & i2;
  Gate orRes = i1 | i2;

  auto addRes = add1Bit(i1, i2, carryIn);

  std::array<Gate, 4> muxIn{andRes, orRes, addRes.result, less};
  Gate result = mux1BitNto1(muxIn, operatorSS);

  return {
      .result = result, .carry_out = addRes.carry_out, .addOut = addRes.result};
}

struct AluBusResult {
  Bus result;
  Gate carry_out;
  Gate overflow;
};

inline AluBusResult aluBus(Bus &in1, Bus &in2, Gate in1SS, Gate in2SS,
                           Gate eqSS, std::array<Gate, 2> operatorSS) {
  Gate carryIn = in2SS;
  Bus resultBus{};
  Gate addOutMSB{};
  Gate overflow{};
  for (int i = 0; i < WORD; ++i) {
    auto res =
        alu1Bit(in1[i], in2[i], {false}, in1SS, in2SS, carryIn, operatorSS);
    if (i == WORD - 1) {
      addOutMSB = res.addOut;
      overflow = xorGate(carryIn, res.carry_out);
    }
    resultBus[i] = res.result;
    carryIn = res.carry_out;
  }
  Gate lessForBit0 = xorGate(overflow, addOutMSB);

  auto res0 =
      alu1Bit(in1[0], in2[0], lessForBit0, in1SS, in2SS, in2SS, operatorSS);

  resultBus[0] = res0.result;

  //for the equal operator
  Gate has1 {};
  for (int i = 0; i < WORD; ++i) {
    has1 = resultBus[i] | has1;
  }
  resultBus[0] = mux1Bit(resultBus[0], has1, eqSS);

  return {.result = resultBus, .carry_out = carryIn, .overflow = overflow};
}
