#pragma once
#include "gates.hpp"
#include <array>

inline Gate mux1Bit(const Gate &in1, const Gate &in2, const Gate &signal) {

  Gate a1 = in1 & (!signal);
  Gate a2 = in2 & signal;
  return a1 | a2;
}

inline Bus muxBus(const Bus &in1, const Bus &in2, const Gate &signal) {
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

template <std::size_t N>
inline Gate addressMatch(const std::array<Gate, N> &address,
                         const uint32_t &num) {
  Gate match{true};
  for (uint32_t j = 0; j < N; ++j) {
    Gate currentBit = {static_cast<bool>((num >> j) & 1)};
    Gate currentBitMatches = !xorGate(address[j], currentBit);
    match = match & currentBitMatches;
  }
  return match;
}

template <std::size_t n>
Gate mux1BitNto1(const std::array<Gate, n> &in1,
                 const std::array<Gate, ceilLog2(n)> &selectorSignal) {
  auto k = ceilLog2(n);

  Gate result{};
  for (uint32_t i = 0; i < n; ++i) {
    Gate match = addressMatch(selectorSignal, i);
    result = result | (match & in1[i]);
  }
  return result;
}

template <std::size_t n>
Bus muxBusNTo1(const std::array<Bus, n> &in1,
               const std::array<Gate, ceilLog2(n)> &selectorSignal) {
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
