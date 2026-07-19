#pragma once
#include "../combinational/adder.hpp"
#include "../combinational/gates.hpp"
#include "memory.hpp"

struct Counter {
private:
  Register r{};

public:
  Bus read() const { return r.read(); }
  // increment, load
  void settle(const Bus &in, const Gate &selectorSignal, const Gate &condition,
              const Gate &useCondition, const Gate &halt) {
    Bus addOne{};
    addOne[0] = {true};
    Bus current = r.read();
    Bus incrementRes = addBus(current, addOne, {false}).result;

    Gate realCondition = mux1Bit({true}, condition, useCondition);

    Bus loadRes = muxBus(incrementRes, in, realCondition);

    Bus newIn = muxBus(incrementRes, loadRes, selectorSignal);

    newIn = muxBus(newIn, current, halt);

    r.settle(newIn, {true});
  }
  void latch() { r.latch(); }
};
