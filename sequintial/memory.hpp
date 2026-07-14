#pragma once
#include "../combinational/gates.hpp"
#include "../combinational/multiplexor.hpp"
#include <array>

struct DFF {
private:
  Gate q; // output
public:
  Gate d; // input

  DFF() : q({false}), d({false}) {};
  DFF(Gate d) : q(d), d(d) {};

  void tick() { q = d; };
  Gate read() const { return q; }
};

struct Register1Bit {
private:
  DFF dff;

public:

  Register1Bit() : load({false}), in({false}), dff(DFF()) {};
  void tick() {
    Gate res = mux1Bit(dff.read(), in, load);
    dff.d = res;
    dff.tick();
  }

  Gate load;
  Gate in;
  Gate read() const { return dff.read(); }
};

struct Register {
private:
  std::array<Register1Bit, WORD> bits;

public:
  Gate load;
  Bus in;
  void tick() {
    for (int i = 0; i < WORD; ++i) {
      Register1Bit &bit = bits[i];
      bit.in = in[i];
      bit.load = load;
      bit.tick();
    }
  }

  Bus read() const {
    Bus output{};
    for (int i = 0; i < WORD; ++i) {
      output[i] = bits[i].read();
    }
    return output;
  }
};

template <std::size_t N> struct RamN {
private:
  std::array<Register, N> registers;
public:
  Bus in;
  Gate load;
  std::array<Gate, ceilLog2(N)> address;
  Bus read() const {
    Bus result {};
    for (uint32_t i = 0; i < N; i++) {
      Gate match = addressMatch(address, i);
      result = result | (registers[i].read() & match);
    }
    return result;
  }
  void tick() {
    for (uint32_t i = 0; i < N; ++i) {
      Register& r = registers[i];

      Gate match = addressMatch(address, i);
      r.in = in;
      r.load = load & match;

      r.tick();
    }
  }
};


