#pragma once
#include "cpu.hpp"
#include "memory.hpp"
#include <array>

struct ComputerState {
  std::array<Bus, 1024> ram;
  CpuState cpu;
};
struct Computer {
private:
  Ram1024 ram{};
  Cpu cpu{};

  void settle() {

    Bus instruction = ram.read(cpu.instructionAddress());
    cpu.settleDecode(instruction);

    Bus memoryReadData = ram.read(cpu.memoryPort.address);
    cpu.settleExecute(instruction, memoryReadData);

    ram.settle(cpu.memoryPort.writeData, cpu.memoryPort.address,
               cpu.memoryPort.writeLoad);
  }
  void latch() {
    ram.latch();
    cpu.latch();
  }

public:
  void tick() {
    settle();
    latch();
  }
  Gate readHalt() const { return cpu.readHalt(); }

  template <std::size_t N>
  void bootLoader(const std::array<Bus, N> &instructions,
                  const std::array<Bus, N> &addresses) {
    for (uint32_t i = 0; i < N; ++i) {
      ram.settle(instructions[i], addresses[i], {true});
      ram.latch();
    }
  }

  ComputerState getComputerState() {
    return {ram.readAll(), cpu.getCpuState()};
  }
};
