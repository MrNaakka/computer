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

  std::array<Gate, ceilLog2(N)> getAddressBits(const Bus & address) {
    std::array<Gate, ceilLog2(N)> a{};
    for (int i = 0; i < ceilLog2(N); ++i) {
      a[i] = address[i];
    }
    return a;
  }

public:
  Bus in{};

  Gate load{};
  Bus writeAddress{};

  Bus read(const std::array<Gate, ceilLog2(N)> &readAddress) const {
    Bus result{};
    for (uint32_t i = 0; i < N; i++) {
      Gate match = addressMatch(readAddress, i);
      result = result | (registers[i].read() & match);
    }
    return result;
  }
  Bus read(const Bus &readAddress) const {
    return read(getAddressBits(readAddress));
  }

  void settle() {
    for (uint32_t i = 0; i < N; ++i) {
      Register &r = registers[i];

      auto wa = getAddressBits(writeAddress);
      Gate match = addressMatch(wa, i);
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
using Ram1024 = RamN<1024>;
