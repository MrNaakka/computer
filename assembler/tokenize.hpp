#pragma once
#include "asm-error.hpp"
#include <cstdint>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct TokenizedLine {
  uint32_t lineNum{};
  std::vector<std::string> words{};
};

inline std::vector<TokenizedLine> tokenize(std::ifstream &is) {

  std::vector<TokenizedLine> result{};
  std::string line;

  uint32_t lineNum = 0;
  while (std::getline(is, line)) {

    TokenizedLine subResult{lineNum};
    std::string word;

    std::istringstream lineStream(line);

    while (lineStream >> word) {
      bool hasHastag = false;
      for (uint32_t i = 0; i < word.size(); ++i) {
        if (word[i] == '#') {
          word.erase(word.begin() + i, word.end());
          hasHastag = true;
        }
      }
      if (!word.empty()) {
        subResult.words.push_back(word);
      }
      if (hasHastag) {
        break;
      }
    }
    lineNum += 1;
    if (subResult.words.size() == 0)
      continue;
    result.push_back(subResult);
  }
  return result;
}

inline std::unordered_map<std::string, uint32_t>
buildSymbolTable(std::vector<TokenizedLine> &lines) {
  std::unordered_map<std::string, uint32_t> result{};
  std::vector<TokenizedLine> keptLines{};
  for (uint32_t i = 0; i < lines.size(); ++i) {
    auto firstWord = lines[i].words[0];
    if (firstWord.size() > 1 && firstWord[firstWord.size() - 1] == ':') {
      firstWord = firstWord.substr(0, firstWord.size() - 1);
      if (result.contains(firstWord)) {
        throw AsmError{
            lines[i].lineNum,
            "This address specifier is already used some where else."};
      }

      result[firstWord] = keptLines.size();
      lines[i].words.erase(lines[i].words.begin());
    }
    if (lines[i].words.size() != 0) {
      keptLines.push_back({lines[i].lineNum, lines[i].words});
    }
  }
  lines = keptLines;

  return result;
}
