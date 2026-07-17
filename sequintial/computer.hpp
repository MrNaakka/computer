#pragma once
#include "../combinational/gates.hpp"
#include "cpu.hpp"
#include "memory.hpp"

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
};
