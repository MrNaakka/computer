#pragma once
#include "../combinational/adder.hpp"
#include "../combinational/gates.hpp"
#include "memory.hpp"

struct Counter {
private:
  Register r{};

public:
  Bus read() const { return r.read(); }
  // increment, load, reset, empty
  void settle(const Bus &in, const std::array<Gate, 2> &selectorSignal,
              const Gate &condition, const Gate &halt) {
    Bus addOne{};
    addOne[0] = {true};
    Bus current = r.read();
    Bus incrementRes = addBus(current, addOne, {false}).result;

    Bus loadRes = muxBus(incrementRes, in, condition);

    Bus resetRes{};

    std::array<Bus, 3> muxIn{incrementRes, loadRes, resetRes};
    Bus newIn = muxBusNTo1(muxIn, selectorSignal);
    newIn = muxBus(newIn, current, halt);

    r.settle(newIn, {true});
  }
  void latch() { r.latch(); }
};
