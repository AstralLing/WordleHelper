#include <array>
#include <cstring>
#include <vector>
#include <fstream>
#include <iostream>

int main() {
  std::array<std::vector<std::string>, 12> DICTIONARY;
  std::ifstream fin;
  std::ofstream fout;
  std::string input;
  fin.open("en.txt");
  while (fin >> input) {
    if (input.size() > 11) continue;
    DICTIONARY[input.size()].push_back(input);
  }
  fin.close();
  std::string requireDic = {"0.txt"};
  for (int i = 4; i <= 11; ++i) {
    requireDic[0] = i - 4 + '0';
    fout.open(requireDic);
    for (auto j : DICTIONARY[i])
      fout << j << std::endl;
    fout.close();
  }
  return 0;
}