#include "_test.hpp"
#include "../multiplexor.hpp"


std::array<Gate, 5> in {Gate{true}, {false}, {false}, {false}, {false}};
std::array<Gate, 3> SS {Gate{false}, {false}, {false}};

bool muxBit1() {
  return mux1BitNto1(in, SS).value == true;
}
bool muxBit2() {
  auto temp = SS;
  temp[0] = {true};
  return mux1BitNto1(in, temp).value == false;
}


void MUXTESTS() {
  CHECK(muxBit1(), "mux1");
  CHECK(muxBit2(), "mux2");
}

