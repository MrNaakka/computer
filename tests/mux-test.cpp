#include "_test.hpp"
#include "../multiplexor.hpp"


void muxBit() {
  std::array<Gate, 5> in{Gate{true}, {false}, {false}, {false}, {false}};
  std::array<Gate, 3> SS{Gate{false}, {false}, {false}};

  CHECK(mux1BitNto1(in, SS).value == true);

  SS[0] = {true};
  CHECK(mux1BitNto1(in, SS).value == false);
}

void MUXTESTS() {
  std::cout << "\n Now running mux tests \n";
  muxBit();
  std::cout << "\n Mux tests finished \n";
}

