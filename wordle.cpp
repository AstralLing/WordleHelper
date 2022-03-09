#include "wlh.h"

class Wordle {
 private:
  const std::string BEST[8] = {"taes",       "crane",      "visaed",
                               "spaeing",    "dioptase",   "kopasetic",
                               "proteinase", "seaborgiums"};
  const int MIN_WORD_LENGTH = 4, MAX_WORD_LENGTH = 11, BEGIN_OPTIMIZE = 2,
            END_OPTIMIZE = 4;

  bool ENABLE_DETAILS = false, ENABLE_OPTIMIZE = false, OPTIMIZE1 = false,
       OPTIMIZE2 = false;

  std::vector<std::string> dictionary, sieve, finder;

  // Customized string.find()
  static inline std::vector<int> findLetter(std::string s, char letter) {
    std::vector<int> result;
    for (int i = 0; i < s.size(); ++i)
      if (s[i] == letter) result.push_back(i);
    return result;
  }

#define USE_BEST_STARTING_WORD

 protected:
  const int MAX_GAME_TURNS = 6;

  // Deny direct access to the dictionary to avoid accidental modifications,
  // only the index is provided.
  int gameTurns = 1, gameWordLength = 5;
  std::string lastGuess, winIdentifier;

  void init(bool showDetail = false, bool useOptimizeSolution = false) {
    if (showDetail) ENABLE_DETAILS = true;
    if (useOptimizeSolution) ENABLE_OPTIMIZE = true;

    std::cout << "* Wordle on! Enter the length of your word.\n";
    while (1) {
      std::cin >> gameWordLength;
      if (gameWordLength < MIN_WORD_LENGTH || gameWordLength > MAX_WORD_LENGTH)
        std::cout << "* Error! Length should be on [4,11].\n";
      else
        break;
    }
    for (int i = 1; i <= gameWordLength; ++i) winIdentifier += '1';

    // Initialize dictionary, source
    // @https://github.com/lorenbrichter/Words/blob/master/Words/en.txt
    std::ifstream fin;
    std::string inputWords;
    std::string requireDic = {"0.txt"};
    requireDic[0] = gameWordLength - 4 + '0';
    fin.open(requireDic);
    while (fin >> inputWords) dictionary.push_back(inputWords);
    fin.close();

    sieve = dictionary;
    finder = dictionary;

    for (int i = 0; i < sieve.size(); ++i) {
      bool illegal = 0;
      for (auto j : sieve[i])
        if (findLetter(sieve[i], j).size() > 1) {
          illegal = 1;
          break;
        }
      if (illegal) {
        std::swap(sieve[i], sieve.back());
        sieve.pop_back();
      }
    }

    return;
  }

  void guess() {
    if (dictionary.empty()) {
      std::cout << "* Oops! No matching words found. Plz add a issue.\n";
      exit(-1);
    }

#ifdef USE_BEST_STARTING_WORD
    if (gameTurns == 1) {
      // Make the first guess. You can modify the "best" starting word
      // as your idea.
      if (ENABLE_DETAILS)
        std::cout << "* Remain " << dictionary.size() << " possible words.\n";

      std::cout << "* Guess: " << BEST[gameWordLength - 4] << "\n";
      lastGuess = BEST[gameWordLength - 4];

      return;
    }
#endif

    // Play
    if (ENABLE_DETAILS)
      std::cout << "* Remain " << dictionary.size() << " possible words.\n";

    bool optimizedFlag = 0;

    if (ENABLE_OPTIMIZE) {
      if (OPTIMIZE1 && !sieve.empty() && dictionary.size() > 20) {
        int guessWordID = randNum(0, (int)sieve.size() - 1);
        std::cout << "* Guess: " << sieve[guessWordID] << "\n";
        lastGuess = sieve[guessWordID];
        optimizedFlag = 1;
      } else if (OPTIMIZE2 && dictionary.size() < 100 &&
                 dictionary.size() > 2) {
        finderWork();
        if (!finder.empty()) {
          int guessWordID = randNum(0, (int)finder.size() - 1);
          std::cout << "* Guess: " << finder[guessWordID] << "\n";
          lastGuess = finder[guessWordID];
          optimizedFlag = 1;
        }
      }
    }

    if (optimizedFlag) {
      if (ENABLE_DETAILS) std::cout << "* Optimized.\n";
    } else {
      int guessWordID = randNum(0, (int)dictionary.size() - 1);
      std::cout << "* Guess: " << dictionary[guessWordID] << "\n";
      lastGuess = dictionary[guessWordID];
    }

    return;
  }

  void finderWork() {
    std::vector<char> letterToBeCheck;
    for (auto i : dictionary) {
      for (auto j : i) {
        auto result = findLetter(lastGuess, j);
        if (result.empty()) letterToBeCheck.push_back(j);
      }
    }
    std::sort(letterToBeCheck.begin(), letterToBeCheck.end());
    letterToBeCheck.erase(
        std::unique(letterToBeCheck.begin(), letterToBeCheck.end()),
        letterToBeCheck.end());
    for (int i = 0; i < finder.size();) {
      int appearCnt = 0;
      for (auto j : finder[i])
        for (auto k : letterToBeCheck)
          if (j == k) ++appearCnt;
      if (appearCnt < (gameWordLength + 2) / 3 || finder[i] == lastGuess) {
        std::swap(finder[i], finder.back());
        finder.pop_back();
      } else {
        ++i;
      }
    }
  }

  void checkNonExist(char letter) {
    if (ENABLE_DETAILS)
      std::cout << "Checking: " << letter << " isn't exist.\n";

    for (int i = 0; i < dictionary.size();) {
      auto result = findLetter(dictionary[i], letter);
      if (result.size() != 0) {
        swap(dictionary[i], dictionary.back());
        dictionary.pop_back();
      } else {
        ++i;
      }
    }

    if (OPTIMIZE1) {
      for (int i = 0; i < sieve.size();) {
        auto result = findLetter(sieve[i], letter);
        if (result.size() != 0) {
          swap(sieve[i], sieve.back());
          sieve.pop_back();
        } else {
          ++i;
        }
      }
    }
  }

  void checkConfirmed(int pos, char letter) {
    if (ENABLE_DETAILS)
      std::cout << "Checking: " << letter << " at " << pos << ".\n";

    for (int i = 0; i < dictionary.size();) {
      if (dictionary[i][pos] != letter) {
        swap(dictionary[i], dictionary.back());
        dictionary.pop_back();
      } else {
        ++i;
      }
    }

    if (OPTIMIZE1) {
      for (int i = 0; i < sieve.size();) {
        auto result = findLetter(sieve[i], letter);
        if (!result.empty()) {
          swap(sieve[i], sieve.back());
          sieve.pop_back();
        } else {
          ++i;
        }
      }
    }
  }

  void checkAppear(std::vector<int> wrongPos, char letter, int appearTimes) {
    if (ENABLE_DETAILS)
      std::cout << "Checking: " << letter << " appears " << appearTimes
                << " times.\n";

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

  void checkMoreThan(char letter, int appearTimes) {
    if (ENABLE_DETAILS)
      std::cout << "Checking: " << letter << " appears no more than "
                << appearTimes << " times.\n";

    for (int i = 0; i < dictionary.size();) {
      auto result = findLetter(dictionary[i], letter);
      if (result.size() > appearTimes) {
        swap(dictionary[i], dictionary.back());
        dictionary.pop_back();
      } else {
        ++i;
      }
    }

    if (OPTIMIZE1) {
      for (int i = 0; i < sieve.size();) {
        auto result = findLetter(sieve[i], letter);
        if (result.size() > appearTimes) {
          swap(sieve[i], sieve.back());
          sieve.pop_back();
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
    int confirmedCnt = 0;

    // Optimization settings
    if (ENABLE_OPTIMIZE) {
      if (gameTurns >= 1 && gameTurns <= END_OPTIMIZE)
        OPTIMIZE1 = true;
      else
        OPTIMIZE1 = false;
    }

    for (int i = 0; i < gameResult.size(); ++i) {
      if (gameResult[i] == '0')
        receivedZero.push_back(guessedWord[i]);
      else if (gameResult[i] == '1')
        checkConfirmed(i, guessedWord[i]), ++letterCnt[guessedWord[i]], ++confirmedCnt;
      else
        wrongPos[guessedWord[i]].push_back(i), ++letterCnt[guessedWord[i]];
    }

    for (auto i : receivedZero)
      if (letterCnt[i] == 0)
        checkNonExist(i);
      else
        checkMoreThan(i, letterCnt[i]);  // Only received zero can get its cnt.

    for (auto i : wrongPos)
      if (i.second.size() > 0)
        checkAppear(i.second, i.first, letterCnt[i.first]);

    if (ENABLE_OPTIMIZE) {
      if ((confirmedCnt >= (gameWordLength + 1) / 2 || gameTurns > END_OPTIMIZE) && gameTurns < MAX_GAME_TURNS - 1)
        OPTIMIZE2 = true, OPTIMIZE1 = false;
      else
        OPTIMIZE2 = false;
    }
  }
};

class newGame : public Wordle {
 private:
  std::string gameResult;

 public:
  void play(int option) {
    std::cout << "- Usage: 0 means not exist, 1 means correct, 2 means wrong "
                 "position. e.g.\"01201\".\n  If guessed right, enter a single "
                 "\"-1\" to close the game.\n";
    init(option & (1 << 10), option & (1 << 11));

    while (gameTurns <= MAX_GAME_TURNS) {
      guess();

      std::cout << "- Enter the result: ";
      std::cin >> gameResult;

      if (gameResult == "-1" || gameResult == winIdentifier) break;

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

    if (gameResult == "-1" || gameResult == winIdentifier) {
      std::cout << "- Niiiiiiiiice!\n";
    } else {
      std::cout << "- Unfortunately, this program cannot solve this problem, "
                   "you can try to modify the random seed or provides the "
                   "answer in issues. The algorithm will be optimized in the "
                   "future.\n";
    }
  }
};

// There is no guarantee that you can get correct answer in this mode
class continueGame : public Wordle {
 private:
  int alreadyTurns;
  std::string userWord, gameResult;

 public:
  void play(int option) {
    std::cout << "- Usage: 0 means not exist, 1 means correct, 2 means wrong "
                 "position. e.g.\"01201\".\n  If guessed right, enter a single "
                 "\"-1\" to close the game.\n";
    init(option & (1 << 10));

    std::cout << "- Enter the times you have guessed.\n";
    do {
      std::cin >> alreadyTurns;
    } while (alreadyTurns < 0 || alreadyTurns >= MAX_GAME_TURNS);

    while (gameTurns <= alreadyTurns) {
      std::cout << "- Enter the word you guessed in turn " << gameTurns
                << ".\n";
      std::cin >> userWord;

      if (userWord.size() != gameWordLength) {
        std::cout << "- Illegal input, try again.\n";
        continue;
      }

      std::cout << "- Enter the result: ";
      std::cin >> gameResult;

      if (gameResult == "-1" || gameResult == winIdentifier) break;

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

      if (gameResult == "-1" || gameResult == winIdentifier) break;

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

    if (gameResult == "-1" || gameResult == winIdentifier) {
      std::cout << "- Niiiiiiiiice!\n";
    } else {
      std::cout << "- Unfortunately, this program cannot solve this problem, "
                   "you can try to modify the random seed. The algorithm will "
                   "be optimized in the future.\n";
    }
  }
};

void playWordle(int option) {
  if (option & (1 << 2)) {
    newGame game;
    game.play(option);
  } else {
    continueGame game;
    game.play(option);
  }
  return;
}