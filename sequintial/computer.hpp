#pragma once
#include "../combinational/gates.hpp"
#include "cpu.hpp"
#include "memory.hpp"

struct Computer {
private:
  Ram1024 ram{};
  Cpu cpu{};

  void settle() {

    cpu.input.instruction = ram.read(cpu.instructionAddress());
    cpu.settleDecode();

    cpu.input.memoryReadData = ram.read(cpu.memoryPort.address);
    cpu.settleExecute();

    ram.load = cpu.memoryPort.writeLoad;
    ram.writeAddress = cpu.memoryPort.address;
    ram.in = cpu.memoryPort.writeData;
    ram.settle();
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
