#pragma once
#include "../combinational/gates.hpp"
#include "../helpers/converters.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include <array>

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
  void bootLoader(const std::array<uint32_t, N>& instructions) {
    std::array<Bus, N> busses = intsToBusses(instructions);
    for (uint32_t i = 0; i < N; ++i) {
      Bus address = intToBus(i);
      ram.settle(busses[i], address, {true});
      ram.latch();
    }
  }
};
