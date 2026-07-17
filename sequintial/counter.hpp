#pragma once
#include "../combinational/adder.hpp"
#include "../combinational/gates.hpp"
#include "memory.hpp"

struct Counter {
private:
  Register r{};

public:
  std::array<Gate, 2> selectorSignal{}; // increment, load, reset, empty
  Bus in{};
  Gate condition;
  Gate halt;

  Bus read() const { return r.read(); }

  void settle() {
    Bus addOne{};
    addOne[0] = {true};
    Bus current = r.read();
    Bus incrementRes = addBus(current, addOne, {false}).result;

    Bus loadRes = muxBus(incrementRes, in, condition);

    Bus resetRes{};

    std::array<Bus, 3> muxIn{incrementRes, loadRes, resetRes};
    Bus newIn = muxBusNTo1(muxIn, selectorSignal);
    newIn = muxBus(newIn, current, halt);

    r.in = newIn;
    r.load = {true};
    r.settle();
  }
  void latch() { r.latch(); }
};
