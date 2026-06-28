#pragma once
#include "gates.hpp"
#include <array>

inline Gate mux1Bit(Gate in1, Gate in2, Gate signal) {

  Gate a1 = in1 & (!signal);
  Gate a2 = in2 & signal;
  return a1 | a2;
}

inline Bus muxBus(Bus &in1, Bus &in2, Gate signal) {
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
