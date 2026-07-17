#pragma once
#include "gates.hpp"
#include <array>

inline Bus load(const Bus &instruction) {
  Bus res{};
  for (int i = 0; i < 16; ++i) {
    res[i] = instruction[i];
  }
  return res;
}

inline Bus loadHigh(const Bus &instruction,
                    const Bus &currentOutputRegisterValue) {
  Bus res{};
  for (int i = 0; i < 16; ++i) {
    res[i] = currentOutputRegisterValue[i];
    res[i + 16] = instruction[i];
  }
  return res;
}
