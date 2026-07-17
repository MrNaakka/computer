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

struct CpuInput {
  Bus instruction{};
  Bus memoryReadData{};
};
struct MemoryPort {
  Bus address{};
  Bus writeData{};
  Gate writeLoad{};
};

struct Cpu {
private:
  Registers16 registers{};
  Counter counter{};
  DFF halt{};

  DecoderResult decodeResult{};
  InputsAndOutputs inputsAndOutputs{};

public:
  CpuInput input{};
  MemoryPort memoryPort{};

  Bus instructionAddress() const { return counter.read(); }

  void settleDecode() {
    decodeResult = instructionDecoder(input.instruction);
    inputsAndOutputs = getInputsAndOutputs(input.instruction, registers);

    memoryPort = {inputsAndOutputs.input1, inputsAndOutputs.input2,
                  decodeResult.writeToAddress};
  }
  void settleExecute() {
    const auto& [instruction, memoryReadData] = input;
    auto [input1, input2, output, outputAddress] = inputsAndOutputs;

    auto aluRes =
        aluBus(input1, input2, decodeResult.acb.in1SS, decodeResult.acb.in2SS,
               decodeResult.acb.eqSS, decodeResult.acb.operatorSS);

    Bus loadRes = load(instruction);
    Bus loadHighRes = loadHigh(instruction, output);

    std::array<Bus, 4> possibleOutputChoices{aluRes.result, loadRes,
                                             loadHighRes, memoryReadData};
    Bus outputRes =
        muxBusNTo1(possibleOutputChoices, decodeResult.whatToWriteToOutput);

    registers.load = decodeResult.writeToOutput & (!decodeResult.haltFlag);
    registers.writeAddress = outputAddress;
    registers.in = outputRes;
    registers.settle();

    counter.selectorSignal = decodeResult.counterSS;
    counter.condition = input2[0];
    counter.in = input1;
    counter.halt = decodeResult.haltFlag;
    counter.settle();

    halt.d = decodeResult.haltFlag;
  }

  void latch() {
    registers.latch();
    counter.latch();

    halt.tick();
  }

  Gate readHalt() const { return halt.read(); }
};
