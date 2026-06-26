#pragma once
#include <iostream>
#include <array>
#include <ostream>
struct Gate {
  bool value;
};

constexpr int WORD = 32;

inline Gate andGate(Gate in1, Gate in2) { return {.value = in1.value && in2.value}; }
inline Gate operator&(Gate in1, Gate in2) { return andGate(in1, in2); }

inline Gate orGate(Gate in1, Gate in2) { return {.value = in1.value || in2.value}; }
inline Gate operator|(Gate in1, Gate in2) { return orGate(in1, in2); }

inline Gate notGate(Gate in) { return {.value = !in.value}; }
inline Gate operator!(Gate in) { return notGate(in); };

inline Gate xorGate(Gate in1, Gate in2) { return ((!in1) & in2) | (in1 & (!in2)); }

using Bus = std::array<Gate, WORD>;

inline std::ostream &operator<<(std::ostream &os, Bus &b) {
  for (int i = WORD - 1; i >= 0; --i) {
    os << b[i].value;
  }
  return os;
}
inline std::ostream &operator<<(std::ostream &os, Gate &g) {
  os << g.value;
  return os;
}


inline Gate operator==(Gate left, Gate right) {
  return !xorGate(left, right);
}

inline Gate operator==(Bus &left, Bus &right)  {
  Gate match = {true};
  for (int i = 0; i < WORD; ++i) {
    match = match & (left[i] == right[i]);
  }
  return match;
}
