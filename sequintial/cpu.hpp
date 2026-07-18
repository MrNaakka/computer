#pragma once
#include "../combinational/alu.hpp"
#include "../combinational/decoder.hpp"
#include "../combinational/gates.hpp"
#include "../combinational/load.hpp"
#include "../combinational/multiplexor.hpp"
#include "counter.hpp"
#include "memory.hpp"

struct InputsAndOutputs {
  Bus input1;
  Bus input2;
  Bus output;
  Bus outputAddress;
};

inline InputsAndOutputs getInputsAndOutputs(const Bus &instruction,
                                            const Registers16 &registers) {
  Bus input1Address{instruction[20], instruction[21], instruction[22],
                    instruction[23]};
  Bus input1 = registers.read(input1Address);

  Bus input2Address{instruction[16], instruction[17], instruction[18],
                    instruction[19]};
  Bus input2 = registers.read(input2Address);

  Bus outputAddress{instruction[24], instruction[25], instruction[26],
                    instruction[27]};
  Bus output = registers.read(outputAddress);

  return {input1, input2, output, outputAddress};
}

struct MemoryPort {
  Bus address{};
  Bus writeData{};
  Gate writeLoad{};
};

struct CpuState {
  std::array<Bus, 16> registers;
  Bus pc;
  Gate halt;
};

struct Cpu {
private:
  Registers16 registers{};
  Counter counter{};
  DFF halt{};

  DecoderResult decodeResult{};
  InputsAndOutputs inputsAndOutputs{};

public:
  MemoryPort memoryPort{};

  Bus instructionAddress() const { return counter.read(); }

  void settleDecode(const Bus &instruction) {
    decodeResult = instructionDecoder(instruction);
    inputsAndOutputs = getInputsAndOutputs(instruction, registers);

    memoryPort = {inputsAndOutputs.input1, inputsAndOutputs.input2,
                  decodeResult.writeToAddress};
  }
  void settleExecute(const Bus &instruction, const Bus &memoryReadData) {
    const auto &[input1, input2, output, outputAddress] = inputsAndOutputs;

    auto aluRes =
        aluBus(input1, input2, decodeResult.acb.in1SS, decodeResult.acb.in2SS,
               decodeResult.acb.eqSS, decodeResult.acb.operatorSS);

    Bus loadRes = load(instruction);
    Bus loadHighRes = loadHigh(instruction, output);

    std::array<Bus, 4> possibleOutputChoices{aluRes.result, loadRes,
                                             loadHighRes, memoryReadData};
    Bus outputRes =
        muxBusNTo1(possibleOutputChoices, decodeResult.whatToWriteToOutput);

    registers.settle(outputRes, outputAddress,
                     decodeResult.writeToOutput & (!decodeResult.haltFlag));

    counter.settle(input1, decodeResult.counterSS, input2[0],
                   decodeResult.haltFlag);

    halt.d = decodeResult.haltFlag;
  }

  void latch() {
    registers.latch();
    counter.latch();

    halt.tick();
  }

  Gate readHalt() const { return halt.read(); }
  CpuState getCpuState() {
    return {registers.readAll(), counter.read(), halt.read()};
  }
};
