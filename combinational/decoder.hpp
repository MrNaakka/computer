#pragma once
#include "gates.hpp"
#include "multiplexor.hpp"
#include <array>

struct AluControlBits {
  std::array<Gate, 2> operatorSS;
  Gate in1SS;
  Gate in2SS;
  Gate eqSS;
};

struct CounterControlBits {
  Gate counterSS;
  Gate useCondition;
  Gate useConstantSlot;
};

struct DecoderResult {
  AluControlBits acb;

  Gate writeToOutput;
  std::array<Gate, 2> whatToWriteToOutput;

  Gate writeToAddress;
  CounterControlBits counterControlBits;

  Gate haltFlag;
};

inline Gate getHaltFlag(const std::array<Gate, 4> &opcode) {
  std::array<Gate, 16> haltSignals{Gate{false}, {false}, {false}, {false},
                                   {false},     {false}, {false}, {false},
                                   {false},     {false}, {false}, {false},
                                   {false},     {false}, {false}, {true}};
  return mux1BitNto1(haltSignals, opcode);
}

inline CounterControlBits
getCounterControlBits(const std::array<Gate, 4> &opcode) {
  std::array<Gate, 16> counterSSSignals{Gate{false}, {false}, {false}, {false},
                                        {false},     {false}, {false}, {false},
                                        {false},     {false}, {false}, {true},
                                        {true},      {true},  {false}, {false}};

  std::array<Gate, 16> useConditonSignals{
      Gate{false}, {false}, {false}, {false}, {false}, {false},
      {false},     {false}, {false}, {false}, {false}, {true},
      {false},     {false}, {false}, {false}};

  std::array<Gate, 16> useConstantSlotSignals{
      Gate{false}, {false}, {false}, {false}, {false}, {false},
      {false},     {false}, {false}, {false}, {false}, {true},
      {true},      {false}, {false}, {false}};

  return {mux1BitNto1(counterSSSignals, opcode),
          mux1BitNto1(useConditonSignals, opcode),
          mux1BitNto1(useConstantSlotSignals, opcode)};
}

// and, or, add, less
// 00, 01, 10, 11
inline AluControlBits getAluControlBits(const std::array<Gate, 4> &opcode) {

  std::array<Gate, 16> in1Signals{Gate{false}, {false}, {false}, {false},
                                  {false},     {false}, {false}, {false},
                                  {true},      {false}, {false}, {false},
                                  {false},     {false}, {false}, {false}};
  Gate in1SS = mux1BitNto1(in1Signals, opcode);

  std::array<Gate, 16> in2Signals{Gate{false}, {false}, {false}, {true},
                                  {true},      {true},  {false}, {false},
                                  {true},      {true},  {false}, {false},
                                  {false},     {false}, {false}, {false}};

  Gate in2SS = mux1BitNto1(in2Signals, opcode);
  std::array<Gate, 16> eqSignals{Gate{false}, {false}, {false}, {false},
                                 {true},      {false}, {false}, {false},
                                 {false},     {false}, {false}, {false},
                                 {false},     {false}, {false}, {false}};
  Gate eqSS = mux1BitNto1(eqSignals, opcode);

  std::array<Gate, 16> operatorbit0Signals{
      Gate{false}, {false}, {false}, {false}, {false}, {true},
      {false},     {true},  {false}, {false}, {false}, {false},
      {false},     {false}, {false}, {false}};

  std::array<Gate, 16> operatorbit1Signals{
      Gate{false}, {false}, {true},  {true},  {true},  {true},
      {false},     {false}, {false}, {false}, {false}, {false},
      {false},     {false}, {false}, {false}};

  std::array<Gate, 2> operatorSS{mux1BitNto1(operatorbit0Signals, opcode),
                                 mux1BitNto1(operatorbit1Signals, opcode)};

  return {operatorSS, in1SS, in2SS, eqSS};
}

inline DecoderResult instructionDecoder(const Bus &instruction) {

  std::array<Gate, 4> opcode{instruction[28], instruction[29], instruction[30],
                             instruction[31]};

  std::array<Gate, 16> writeToOutputSignals{
      Gate{true}, {true},  {true}, {true},  {true},  {true},  {true},  {true},
      {true},     {false}, {true}, {false}, {false}, {false}, {false}, {false}};
  Gate writeToOutput = mux1BitNto1(writeToOutputSignals, opcode);

  // alu group, load group, load high group, read group
  // 00, 01, 10, 11

  std::array<Gate, 16> whatToWriteToOutputSignal0Bit{
      Gate{true}, {false}, {false}, {false}, {false}, {false},
      {false},    {false}, {false}, {false}, {true},  {false},
      {false},    {false}, {false}, {false}};

  std::array<Gate, 16> whatToWriteToOutputSignal1Bit{
      Gate{false}, {true},  {false}, {false}, {false}, {false},
      {false},     {false}, {false}, {false}, {true},  {false},
      {false},     {false}, {false}, {false}};

  std::array<Gate, 2> whatToWriteToOutput{
      mux1BitNto1(whatToWriteToOutputSignal0Bit, opcode),
      mux1BitNto1(whatToWriteToOutputSignal1Bit, opcode)};

  std::array<Gate, 16> writeToAddressSignals{
      Gate{false}, {false}, {false}, {false}, {false}, {false},
      {false},     {false}, {false}, {true},  {false}, {false},
      {false},     {false}, {false}, {false}};

  Gate writeToAddress = mux1BitNto1(writeToAddressSignals, opcode);

  return {getAluControlBits(opcode),     writeToOutput,
          whatToWriteToOutput,           writeToAddress,
          getCounterControlBits(opcode), getHaltFlag(opcode)};
}
