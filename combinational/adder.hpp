
#pragma once
#include "multiplexor.hpp"
#include "gates.hpp"
template <typename T> struct WithCarrieOut {
  T result;
  Gate carry_out;
};

inline WithCarrieOut<Gate> add1Bit(Gate in1, Gate in2, Gate carry_in) {



  Gate result = xorGate(xorGate(in1, in2), carry_in);
  Gate carry_out = (in1 & in2) | (in1 & carry_in) | (in2 & carry_in);
  return {result, carry_out};
}

inline WithCarrieOut<Bus> addBus(Bus &in1, Bus &in2, Gate signal) {

  Bus result;
  Gate carry_in = signal;

  for (int i = 0; i < WORD; ++i) {

    auto correctIn2 = mux1Bit(in2[i], !in2[i], signal);

    auto [res, carry_out] = add1Bit(in1[i], correctIn2, carry_in);
    result[i] = res;
    carry_in = carry_out;
  }

  return {.result = result, .carry_out = carry_in};
}
