#pragma once
#include <iostream>
#include <string>

inline void CHECK(bool a, std::string name = "") {
  if (a) {
    std::cout << name << " passed the test" << std::endl;
  } else {
    std::cout << name << " failed the test" << std::endl;
  }
}
