
#include <array>
#include <cstdint>
#include <iostream>
#include <ostream>

struct Gate {
  bool value;
};

constexpr int WORD = 32;

Gate andGate(Gate in1, Gate in2) { return {.value = in1.value && in2.value}; }
Gate operator&(Gate in1, Gate in2) { return andGate(in1, in2); }

Gate orGate(Gate in1, Gate in2) { return {.value = in1.value || in2.value}; }
Gate operator|(Gate in1, Gate in2) { return orGate(in1, in2); }

Gate notGate(Gate in) { return {.value = !in.value}; }
Gate operator!(Gate in) { return notGate(in); };

Gate xorGate(Gate in1, Gate in2) { return ((!in1) & in2) | (in1 & (!in2)); }

using Bus = std::array<Gate, WORD>;

std::ostream &operator<<(std::ostream &os, Bus &b) {
  for (int i = WORD - 1; i >= 0; --i) {
    os << b[i].value;
  }
  return os;
}

Gate mux1Bit(Gate in1, Gate in2, Gate signal) {

  Gate a1 = in1 & (!signal);
  Gate a2 = in2 & signal;
  return a1 | a2;
}

Bus muxBus(Bus &in1, Bus &in2, Gate signal) {
  Bus result;
  for (int i = 0; i < WORD; ++i) {
    result[i] = mux1Bit(in1[i], in2[i], signal);
  }
  return result;
}

constexpr std::size_t ceilLog2(uint32_t n) {
  std::size_t bits = 0;
  while ((uint32_t{1} << bits) < n) {
    bits++;
  }
  return bits;
}

template <std::size_t n>
Gate mux1BitNto1(std::array<Gate, n> &in1,

                 std::array<Gate, ceilLog2(n)> &selectorSignal) {
  auto k = ceilLog2(n);

  Gate result{};
  for (uint32_t i = 0; i < n; ++i) {
    Gate match{.value = true};
    for (uint32_t j = 0; j < k; ++j) {

      Gate currentBit = {.value = static_cast<bool>((i >> j) & 1)};
      auto currentBitMatches = !xorGate(selectorSignal[j], currentBit);
      match = match & currentBitMatches;
    }
    result = result | (match & in1[i]);
  }
  return result;
}

template <std::size_t n>
Bus muxBusNTo1(std::array<Bus, n> &in1,
               std::array<Gate, ceilLog2(n)> &selectorSignal) {
  Bus result{};

  std::array<Gate, n> currentIndexGates;
  for (int i = 0; i < WORD; ++i) {
    for (int j = 0; j < n; ++j) {
      currentIndexGates[j] = in1[j][i];
    }

    result[i] = mux1BitNto1(currentIndexGates, selectorSignal);
  }

  return result;
}

template <typename T> struct WithCarrieOut {
  T result;
  Gate carry_out;
};

WithCarrieOut<Gate> add1Bit(Gate in1, Gate in2, Gate carry_in) {
  Gate result = xorGate(xorGate(in1, in2), carry_in);
  Gate carry_out = (in1 & in2) | (in1 & carry_in) | (in2 & carry_in);
  return {result, carry_out};
}

WithCarrieOut<Bus> add32Bits(Bus &in1, Bus &in2, Gate signal) {

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

WithCarrieOut<Gate> alu1Bit(Gate in1, Gate in2, Gate in1SS, Gate in2SS,
                            Gate carryIn, std::array<Gate, 2> operatorSS) {

  Gate i1 = mux1Bit(in1, in2, in1SS);
  Gate i2 = mux1Bit(in1, in2, in2SS);

  Gate andRes = i1 & i2;
  Gate orRes = i1 | i2;

  auto addRes = add1Bit(in1, in2, carryIn);

  std::array<Gate, 3> muxIn{andRes, orRes, addRes.result};
  Gate result = mux1BitNto1(muxIn, operatorSS);
  return {.result = result, .carry_out = addRes.carry_out};
}

WithCarrieOut<Bus> alu32Bit(Bus in1, Bus in2, Gate in1SS, Gate in2SS,
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

int main() {

  std::array<Bus, 4> options{};
  std::array<Gate, 2> sel{};
  auto picked = muxBusNTo1(options, sel);

  return 0;
}
