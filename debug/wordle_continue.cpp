#include <array>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <vector>

// "WLH.h"
unsigned int simpleHash(char *s) {
  unsigned int result = 0;
  for (int i = 0; i < strlen(s); ++i)
    result = (result << 11) ^ (result >> 4) ^ s[i];
  return result ^ (result >> 5) ^ (result >> 14);
}
std::mt19937 randGenerator(simpleHash("NeverKnowsBest."));
template <typename T>
inline T randNum(T l, T r) {
  return std::uniform_int_distribution<T>(l, r)(randGenerator);
}

// Wordle main
class Wordle {
 private:
  const std::string BEST[8] = {"taes",       "crane",      "visaed",
                               "spaeing",    "dioptase",   "kopasetic",
                               "proteinase", "seaborgiums"};
  const int MIN_WORD_LENGTH = 4, MAX_WORD_LENGTH = 11;

  std::vector<std::string> dictionary;

  // Customized string.find()
  static inline std::vector<int> findLetter(std::string s, char letter) {
    std::vector<int> result;
    for (int i = 0; i < s.size(); ++i)
      if (s[i] == letter) result.push_back(i);
    return result;
  }

#define USE_BEST_STARTING_WORD

 protected:
  // Deny direct access to the dictionary to avoid accidental modifications,
  // only the index is provided.
  int gameTurns = 1, gameWordLength = 5;
  std::string lastGuess;

  void init() {
    std::cout << "Wordle on! Enter the length of your word.\n";
    while (1) {
      std::cin >> gameWordLength;
      if (gameWordLength < MIN_WORD_LENGTH || gameWordLength > MAX_WORD_LENGTH)
        std::cout << "Error! Length should be on [4,11].\n";
      else
        break;
    }

    // Initialize dictionary, source
    // @https://github.com/lorenbrichter/Words/blob/master/Words/en.txt
    std::ifstream fin;
    std::string inputWords;
    std::string requireDic = {"0.txt"};
    requireDic[0] = gameWordLength - 4 + '0';
    fin.open(requireDic);
    while (fin >> inputWords) dictionary.push_back(inputWords);
    fin.close();

    return;
  }

  void guess() {
    if (dictionary.empty()) {
      std::cout << "* Oops! Unknown error. Restart this program and try "
                   "again.\n";
      return;
    }

#ifdef USE_BEST_STARTING_WORD
    if (gameTurns == 1) {
      // Make the first guess. You can modify the "best" starting word
      // as your idea.
      std::cerr << "* Remain " << dictionary.size() << " possible words.\n";
      std::cout << "* Guess: " << BEST[gameWordLength - 4] << "\n";
      lastGuess = BEST[gameWordLength - 4];

      return;
    }
#endif

    // Play
    std::cerr << "* Remain " << dictionary.size() << " possible words.\n";
    int guessWordID = randNum(0, (int)dictionary.size() - 1);
    std::cout << "* Guess: " << dictionary[guessWordID] << "\n";
    lastGuess = dictionary[guessWordID];

    return;
  }

  void checkNonExist(char letter) {
    // std::cerr << "Checking: " << letter << " doesn't exist.\n";
    for (int i = 0; i < dictionary.size();) {
      auto result = findLetter(dictionary[i], letter);
      if (result.size() != 0) {
        swap(dictionary[i], dictionary.back());
        dictionary.pop_back();
      } else {
        ++i;
      }
    }
  }

  void checkConfirmed(int pos, char letter) {
    // std::cerr << "Checking: " << letter << " is at " << pos << ".\n";
    for (int i = 0; i < dictionary.size();) {
      if (dictionary[i][pos] != letter) {
        swap(dictionary[i], dictionary.back());
        dictionary.pop_back();
      } else {
        ++i;
      }
    }
  }

  void checkAppear(std::vector<int> wrongPos, char letter, int appearTimes) {
    // std::cerr << "Checking: " << letter << " appears " << appearTimes << "
    // times.\n";
    for (int i = 0; i < dictionary.size();) {
      auto result = findLetter(dictionary[i], letter);
      if (result.size() < appearTimes) {  // Note: not wrongPos.size()
        swap(dictionary[i], dictionary.back());
        dictionary.pop_back();
      } else {
        bool illegal = 0;
        for (auto j : wrongPos)
          if (dictionary[i][j] == letter) {
            illegal = 1;
            break;
          }
        if (illegal) {
          swap(dictionary[i], dictionary.back());
          dictionary.pop_back();
        } else {
          ++i;
        }
      }
    }
  }

  void solveGame(std::string guessedWord, std::string gameResult) {
    std::map<char, std::vector<int>> wrongPos;
    std::map<char, int> letterCnt;
    std::vector<char> receivedZero;

    for (int i = 0; i < gameResult.size(); ++i) {
      if (gameResult[i] == '0')
        receivedZero.push_back(guessedWord[i]);
      else if (gameResult[i] == '1')
        checkConfirmed(i, guessedWord[i]), ++letterCnt[guessedWord[i]];
      else
        wrongPos[guessedWord[i]].push_back(i), ++letterCnt[guessedWord[i]];
    }

    for (auto i : receivedZero)
      if (letterCnt[i] == 0) checkNonExist(i);

    for (auto i : wrongPos)
      if (i.second.size() > 0)
        checkAppear(i.second, i.first, letterCnt[i.first]);
  }
};

// There is no guarantee that you can get correct answer in this mode
class continueGame : public Wordle {
 private:
  const int MAX_GAME_TURNS = 6;

  int alreadyTurns;
  std::string userWord, gameResult;

 public:
  void play() {
    std::cout << "- Usage: 0 means not exist, 1 means correct, 2 means wrong "
              "position. e.g.\"01201\".\n  If guessed right, enter a single "
              "\"-1\" to close the game.\n";
    init();

    std::cout << "- Enter the times you have guessed.\n";
    std::cin >> alreadyTurns;

    while (gameTurns <= alreadyTurns) {
      std::cout << "- Enter the word you guessed in turn " << gameTurns << ".\n";
      std::cin >> userWord;
      
      if (userWord.size() != gameWordLength) {
        std::cout << "- Illegal input, try again.\n";
        continue;
      }

      std::cout << "- Enter the result: ";
      std::cin >> gameResult;

      if (gameResult == "-1") break;

      if (gameResult.size() != gameWordLength) {
        std::cout << "- Illegal input, try again.\n";
        continue;
      }

      bool illegal = 0;
      for (auto i : gameResult)
        if (i != '0' && i != '1' && i != '2') {
          illegal = 1;
          break;
        }
      if (illegal) {
        std::cout << "- Illegal input, try again.\n";
        continue;
      }

      solveGame(userWord, gameResult);

      ++gameTurns;
    }

    std::cout << "- Data loaded, program start.\n";

    while (gameTurns <= MAX_GAME_TURNS) {
      guess();

      std::cout << "- Enter the result: ";
      std::cin >> gameResult;

      if (gameResult == "-1") break;

      if (gameResult.size() != gameWordLength) {
        std::cout << "- Illegal input, try again.\n";
        continue;
      }

      bool illegal = 0;
      for (auto i : gameResult)
        if (i != '0' && i != '1' && i != '2') {
          illegal = 1;
          break;
        }
      if (illegal) {
        std::cout << "- Illegal input, try again.\n";
        continue;
      }

      solveGame(lastGuess, gameResult);

      ++gameTurns;
    }

    if (gameResult == "-1") {
      std::cout << "- Niiiiiiiiice!\n";
    } else {
      std::cout << "- Unfortunately, this program cannot solve this problem, "
                   "you can try to modify the random seed. The algorithm will "
                   "be optimized in the future.\n";
    }
  }
};

// Debug
int main() {
  continueGame game;
  game.play();
  return 0;
}