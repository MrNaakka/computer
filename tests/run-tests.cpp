#include "_test.hpp"
#include "mux-test.cpp"
#include "alu-test.cpp"

int main() {

  MUXTESTS();
  ALUTESTS();


  testsSummary();
  return 0;

}
