#pragma once
#include <iostream>
#include <string>

struct TestResults {
  int totalTests;
  int totalPassedTests;
};

inline TestResults &getCurrentTestsResults() {
  static TestResults r{0, 0};
  return r;
}

inline void helper(std::string name, bool res, int line) {
  TestResults &t = getCurrentTestsResults();
  t.totalTests++;
  if (res) {
    t.totalPassedTests++;
    std::cout << name << " passed" << std::endl;
  } else {
    std::cout << "\n------------------------------------------------------"
              << std::endl;

    std::cout << name << " failed on line: " << line << std::endl;

    std::cout << "------------------------------------------------------"
              << std::endl;
  }
}

#define CHECK(exec) helper(#exec, exec, __LINE__)

inline void testsSummary() {
  std::cout
      << "\n\n\n------------------------------------------------------\n\n\n";

  TestResults &t = getCurrentTestsResults();
  std::cout << "Total number of tests: " << t.totalTests << std::endl;
  std::cout << "Total number of passed tests: " << t.totalPassedTests
            << std::endl;
  int failedNum = t.totalTests - t.totalPassedTests;
  std::cout << "Total number of failed tests: " << failedNum << std::endl;
}
