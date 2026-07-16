#pragma once
#include "../combinational/gates.hpp"
#include "../combinational/multiplexor.hpp"
#include <array>

// assuming only a d-flip-flop
// every other memory element is built on top of that usign combinational logic

struct DFF {
private:
  Gate q{}; // output
public:
  Gate d{}; // input

  DFF() {};
  DFF(Gate d) : q(d), d(d) {};

  void tick() { q = d; };
  Gate read() const { return q; }
};

struct Register1Bit {
private:
  DFF dff{};

public:
  void settle() { dff.d = mux1Bit(dff.read(), in, load); }
  void latch() { dff.tick(); }

  Gate load{};
  Gate in{};
  Gate read() const { return dff.read(); }
};

struct Register {
private:
  std::array<Register1Bit, WORD> bits{};

public:
  Gate load{};
  Bus in{};

  void settle() {
    for (int i = 0; i < WORD; ++i) {
      Register1Bit &bit = bits[i];
      bit.in = in[i];
      bit.load = load;
      bit.settle();
    }
  }
  void latch() {
    for (int i = 0; i < WORD; ++i) {
      Register1Bit &bit = bits[i];
      bit.latch();
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
  std::array<Register, N> registers{};

public:
  Bus in{};

  Gate load{};
  std::array<Gate, ceilLog2(N)> writeAddress{};

  Bus read(const std::array<Gate, ceilLog2(N)> &readAddress) const {
    Bus result{};
    for (uint32_t i = 0; i < N; i++) {
      Gate match = addressMatch(readAddress, i);
      result = result | (registers[i].read() & match);
    }
    return result;
  }
  void settle() {
    for (uint32_t i = 0; i < N; ++i) {
      Register &r = registers[i];

      Gate match = addressMatch(writeAddress, i);
      r.in = in;
      r.load = load & match;

      r.settle();
    }
  }
  void latch() {
    for (uint32_t i = 0; i < N; ++i) {
      Register &r = registers[i];
      r.latch();
    }
  }
};

using Registers16 = RamN<16>;
