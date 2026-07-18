#pragma once
#include "../combinational/gates.hpp"
#include <cstdint>

inline Bus intToBus(const uint32_t &num) {
  Bus res{};
  for (int i = 0; i < 32; ++i) {
    res[i] = {static_cast<bool>((num >> i) & 1)};
  }
  return res;
}

template <std::size_t N>
std::array<Bus, N> intsToBusses(const std::array<uint32_t, N> &nums) {
  std::array<Bus, N> res{};
  for (uint32_t i = 0; i < N; ++i) {
    res[i] = intToBus(nums[i]);
  }
  return res;
}

inline uint32_t busToInt(const Bus &b) {
  uint32_t res{};
  for (uint32_t i = 0; i < WORD; ++i) {
    res = res | (static_cast<uint32_t>(b[i].value) << i);
  }
  return res;
}

template <std::size_t N>
std::array<uint32_t, N> bussesToInts(const std::array<Bus, N> &busses) {
  std::array<uint32_t, N> res{};
  for (uint32_t i = 0; i < N; ++i) {
    res[i] = busToInt(busses[i]);
  }
  return res;
}
