#pragma once
#include "../combinational/gates.hpp"
#include "../sequintial/computer.hpp"
#include "converters.hpp"
#include <cstdint>

struct CpuSnapshot {
  std::array<uint32_t, 16> registers;
  uint32_t pc;
  bool halt;
};

inline CpuSnapshot cpuStateToCpuSnapshot(const CpuState &cpu) {
  return {bussesToInts(cpu.registers), busToInt(cpu.pc), cpu.halt.value};
}

struct ComputerSnapshot {
  std::array<uint32_t, 1024> ram;
  CpuSnapshot cpu;
};

inline ComputerSnapshot
computerStateToComputerSnapshot(const ComputerState &c) {
  return {bussesToInts(c.ram), cpuStateToCpuSnapshot(c.cpu)};
}
