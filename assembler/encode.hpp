#pragma once
#include "asm-error.hpp"
#include "tokenize.hpp"
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

enum class Slot { Output, Input1, Input2, BothInputs, Const };

struct OpSpec {
  uint32_t opcode;
  std::vector<Slot> slots;
};

inline OpSpec group1(uint32_t opcode) {
  return {opcode, {Slot::Output, Slot::Input1, Slot::Input2}};
}
inline OpSpec group2(uint32_t opcode) {
  return {opcode, {Slot::Output, Slot::Const}};
}
inline OpSpec group3(uint32_t opcode) {
  return {opcode, {Slot::Output, Slot::BothInputs}};
}
inline OpSpec group4(uint32_t opcode) {
  return {opcode, {Slot::Input1, Slot::Input2}};
}
inline OpSpec group5(uint32_t opcode) {
  return {opcode, {Slot::Output, Slot::Input1}};
}

inline OpSpec group6(uint32_t opcode) {
  return {opcode, {Slot::Input2, Slot::Const}};
}
inline OpSpec group7(uint32_t opcode) { return {opcode, {Slot::Const}}; }
inline OpSpec group8(uint32_t opcode) { return {opcode, {Slot::Input1}}; }
inline OpSpec group9(uint32_t opcode) { return {opcode, {}}; }

const std::unordered_map<std::string, OpSpec> IsaSpecs = {
    {"load", group2(0)},  {"load-high", group2(1)},
    {"add", group1(2)},   {"sub", group1(3)},
    {"eq", group1(4)},    {"lt", group1(5)},
    {"and", group1(6)},   {"or", group1(7)},
    {"not", group3(8)},   {"write", group4(9)},
    {"read", group5(10)}, {"jmpc", group6(11)},
    {"jmpa", group7(12)}, {"jmpr", group8(13)},
    {"halt", group9(15)}

};

inline uint32_t regToNum(const std::string &word, uint32_t lineNum) {
  if (word[0] != 'r') {
    throw AsmError{lineNum, "'" + word + "': must be a register."};
  }
  std::string registerNumAsString = word.substr(1);
  uint32_t registerNum;
  try {
    registerNum = std::stoi(registerNumAsString);
  } catch (const std::exception &e) {
    throw AsmError{lineNum, "'" + registerNumAsString +
                                "':You must specify the register number. "
                                "Register values go from 0-15."};
  }
  if (registerNum > 15) {
    throw AsmError{lineNum, "'" + registerNumAsString +
                                "':Register number is out of bounds"
                                "Register values go from 0-15."};
  }
  return registerNum;
}

inline uint32_t constToNum(const std::string &word, uint32_t lineNum) {

  uint32_t constant;
  try {
    constant = std::stoi(word);
  } catch (const std::exception &e) {
    throw AsmError{lineNum, "'" + word + "': is not a valid constant"};
  }
  if (constant > (0xFFFF)) {

    throw AsmError{lineNum,
                   "'" + word +
                       "': is out of bounds. It must be less than 2 << 16."};
  }
  return constant;
}

struct InstructionParts {
  uint32_t opcode{};
  uint32_t output{};
  uint32_t input1{};
  uint32_t input2{};
  uint32_t constant{};
};
inline uint32_t instructionPartsToInstruction(const InstructionParts &ip) {
  auto [opcode, output, i1, i2, c] = ip;
  return 0 | (opcode << 28) | (output << 24) | (i1 << 20) | (i2 << 16) | c;
}

inline uint32_t
encodeLine(const std::vector<std::string> &line, uint32_t lineNum,
           const std::unordered_map<std::string, uint32_t> &symbolTable) {
  if (line.empty()) {
    throw AsmError{lineNum, "Line does not have an instruction."};
  }

  auto opSpec = IsaSpecs.find(line[0]);
  if (opSpec == IsaSpecs.end()) {
    throw AsmError{lineNum, "'" + line[0] + "': does not match any opcode."};
  }

  auto slots = opSpec->second.slots;
  InstructionParts ip{opSpec->second.opcode};
  for (uint32_t i = 0; i < slots.size(); ++i) {
    uint32_t value;
    if (slots[i] == Slot::Const) {

      if (symbolTable.contains(line[i + 1])) {
        value = symbolTable.at(line[i + 1]);

      } else {

        value = constToNum(line[i + 1], lineNum);
      }
    } else {

      value = regToNum(line[i + 1], lineNum);
    }

    switch (slots[i]) {
    case (Slot::Output): {
      ip.output = value;
      break;
    }
    case (Slot::Input1): {
      ip.input1 = value;
      break;
    }
    case (Slot::Input2): {
      ip.input2 = value;
      break;
    }
    case (Slot::Const): {
      ip.constant = value;
      break;
    }
    case (Slot::BothInputs): {
      ip.input1 = value;
      ip.input2 = value;
      break;
    }
    }
  }
  return instructionPartsToInstruction(ip);
}

inline std::vector<uint32_t>
encode(const std::vector<TokenizedLine> &lines,
       const std::unordered_map<std::string, uint32_t> &symbolTable) {
  std::vector<uint32_t> result{};
  for (uint32_t i = 0; i < lines.size(); ++i) {
    auto line = lines[i];
    result.push_back(encodeLine(line.words, line.lineNum, symbolTable));
  }
  return result;
}
