#include "./combinational/alu.hpp"
#include "./combinational/decoder.hpp"
#include "./combinational/gates.hpp"
#include "./combinational/load.hpp"
#include "./combinational/multiplexor.hpp"
#include "./sequintial/counter.hpp"
#include "./sequintial/memory.hpp"

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

struct Cpu {
private:
  Registers16 registers{};
  Counter counter{};

public:
  Ram1024 ram{};
  void settle() {

    Bus instructionAddress = counter.read();
    Bus instruction = ram.read(instructionAddress);
    auto controlFlags = instructionDecoder(instruction);

    auto [input1, input2, output, outputAddress] =
        getInputsAndOutputs(instruction, registers);

    auto aluRes =
        aluBus(input1, input2, controlFlags.acb.in1SS, controlFlags.acb.in2SS,
               controlFlags.acb.eqSS, controlFlags.acb.operatorSS);
    Bus loadRes = load(instruction);
    Bus loadHighRes = loadHigh(instruction, output);
    Bus readRes = ram.read(input1);

    std::array<Bus, 4> possibleOutputChoices{aluRes.result, loadRes,
                                             loadHighRes, readRes};
    Bus outputRes =
        muxBusNTo1(possibleOutputChoices, controlFlags.whatToWriteToOutput);

    registers.load = controlFlags.writeToOutput;
    registers.writeAddress = outputAddress;
    registers.in = outputRes;
    registers.settle();

    counter.selectorSignal = controlFlags.counterSS;
    counter.condition = input2[0];
    counter.in = input1;
    counter.settle();

    ram.writeAddress = input1;
    ram.in = input2;
    ram.load = controlFlags.writeToAddress;

    ram.settle();
  }

  void latch() {
    registers.latch();
    counter.latch();
    ram.latch();
  }
};
