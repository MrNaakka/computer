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
  void settle(const Gate &in, const Gate &load) {
    dff.d = mux1Bit(dff.read(), in, load);
  }
  void latch() { dff.tick(); }

  Gate read() const { return dff.read(); }
};

struct Register {
private:
  std::array<Register1Bit, WORD> bits{};

public:
  void settle(const Bus &in, const Gate &load) {
    for (int i = 0; i < WORD; ++i) {
      Register1Bit &bit = bits[i];
      bit.settle(in[i], load);
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

  std::array<Gate, ceilLog2(N)> getAddressBits(const Bus &address) const {
    std::array<Gate, ceilLog2(N)> a{};
    for (int i = 0; i < ceilLog2(N); ++i) {
      a[i] = address[i];
    }
    return a;
  }

public:
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

  void settle(const Bus &in, const Bus &writeAddress, const Gate &load) {
    auto wa = getAddressBits(writeAddress);
    for (uint32_t i = 0; i < N; ++i) {
      Register &r = registers[i];
      Gate match = addressMatch(wa, i);

      r.settle(in, load & match);
    }
  }
  void latch() {
    for (uint32_t i = 0; i < N; ++i) {
      Register &r = registers[i];
      r.latch();
    }
  }
  std::array<Bus, N> readAll() const {
    std::array<Bus, N> result{};
    for (int i = 0; i < N; ++i) {
      result[i] = registers[i].read();
    }
    return result;
  }
};

using Registers16 = RamN<16>;
using Ram1024 = RamN<1024>;
