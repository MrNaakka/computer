#pragma once
#include <array>
#include <iostream>
#include <ostream>
struct Gate {
  bool value{};
};

constexpr int WORD = 32;

// only assumption is the nAndGate. Everything else is derived from that.
inline Gate nAndGate(const Gate &in1, const Gate &in2) {
  return {!(in1.value && in2.value)};
}

inline Gate notGate(const Gate &in) { return nAndGate(in, in); }
inline Gate operator!(const Gate &in) { return notGate(in); };

inline Gate andGate(const Gate &in1, const Gate &in2) {
  return !nAndGate(in1, in2);
}
inline Gate operator&(const Gate &in1, const Gate &in2) {
  return andGate(in1, in2);
}

inline Gate orGate(const Gate &in1, const Gate &in2) {
  return !((!in1) & (!in2));
}
inline Gate operator|(const Gate &in1, const Gate &in2) {
  return orGate(in1, in2);
}

inline Gate xorGate(const Gate &in1, const Gate &in2) {
  return ((!in1) & in2) | (in1 & (!in2));
}

using Bus = std::array<Gate, WORD>;

inline std::ostream &operator<<(std::ostream &os, const Bus &b) {
  for (int i = WORD - 1; i >= 0; --i) {
    os << b[i].value;
  }
  return os;
}
inline std::ostream &operator<<(std::ostream &os, const Gate &g) {
  os << g.value;
  return os;
}

inline Gate operator==(const Gate &left, const Gate &right) {
  return !xorGate(left, right);
}

inline Gate operator==(const Bus &left, const Bus &right) {
  Gate match = {true};
  for (int i = 0; i < WORD; ++i) {
    match = match & (left[i] == right[i]);
  }
  return match;
}

inline Bus operator&(const Bus &left, const Gate &right) {
  Bus result{};
  for (int i = 0; i < WORD; ++i) {
    result[i] = left[i] & right;
  }
  return result;
}
