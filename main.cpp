#include "wlh.h"

unsigned int simpleHash(char *s) {
  unsigned int result = 0;
  for (int i = 0; i < strlen(s); ++i)
    result = (result << 11) ^ (result >> 4) ^ s[i];
  return result ^ (result >> 5) ^ (result >> 14);
}
std::mt19937 randGenerator(simpleHash("NeverKnowsBest."));
int randNum(int l, int r) { return std::uniform_int_distribution<int>(l, r)(randGenerator); }

int main(int argc, char *argv[]) {
  char optionName;
  int option = 0;

  // Run without args
  if (argc == 1) {
    playWordle((1 << 2) | (1 << 11));
    return 0;
  }

  while (--argc > 0 && (*++argv)[0] == '-') {
    while (optionName = *++argv[0]) {
      switch (optionName) {
        case 'n': // New game
          option |= 1 << 2;
          break;
        case 'c': // Continue
          option |= 1 << 3;
          break;
        case 'd':
          option |= 1 << 10;
          break;
        case 'o':
          option |= 1 << 11;
          break;
        case 'h':
          option |= 1 << 30;
          break;
        default:
          printf("Found: illegal option -%c\n", optionName);
          argc = 0;
          option |= 1 << 31;
          break;
      }
    }
  }

  if (option >> 31) {
    std::cout << "Illegal option found, try again or use -help for help.\n";
    return 0;
  }
  if (option & (1 << 2) && option & (1 << 3)) {
    std::cout << "Option conflicted, use -help for help.\n";
    return 0;
  }
  if (option & (1 << 30)) {
    std::cout << "    -n: start a new game, the program will solve it "
                 "automatically. (default)\n"
                 "    -c: continue your game, input the information you "
                 "already got and the program will give advice.\n"
                 "    -d: show details during game.\n"
                 "    -o: use experimental optimized algorithm, probably "
                 "only works better when the word is short. (Not available "
                 "yet with -c)";
    return 0;
  }

  // Set default
  if (!((option & (1 << 2)) | (option & (1 << 3))))
    option |= 1 << 2;
  playWordle(option);

  return 0;
}