#pragma once
#include <cstdint>
#include <string>

struct AsmError {
  uint32_t line;
  std::string message;
};

