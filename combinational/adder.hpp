
#pragma once
#include "gates.hpp"
#include "multiplexor.hpp"
template <typename T> struct WithCarrieOut {
  T result;
  Gate carry_out;
};

inline WithCarrieOut<Gate> add1Bit(const Gate &in1, const Gate &in2,
                                   const Gate &carry_in) {
  Gate result = xorGate(xorGate(in1, in2), carry_in);
  Gate carry_out = (in1 & in2) | (in1 & carry_in) | (in2 & carry_in);
  return {result, carry_out};
}

inline WithCarrieOut<Bus> addBus(const Bus &in1, const Bus &in2,
                                 const Gate &signal) {

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
