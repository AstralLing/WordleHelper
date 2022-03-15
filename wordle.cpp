#include "wlh.h"
using std::cin;
using std::cout;
using std::string;
using std::vector;

class Wordle {
 private:
  const string BEST[8] = {"taes",     "crane",     "visaed",     "spaeing",
                          "dioptase", "kopasetic", "proteinase", "seaborgiums"};
  const int MIN_WORD_LENGTH = 4, MAX_WORD_LENGTH = 11, BEGIN_OPTIMIZE1 = 1,
            END_OPTIMIZE1 = 3;  // Begin and end pos are both effective in the
                                // next turn (actually [2,4])

  // Optimize1: tmp cancel the limit of '2'. Optimize2: Restrict the search to
  // the unsure letters.
  bool ENABLE_DETAILS = false, ENABLE_OPTIMIZE = false, OPTIMIZE1 = false,
       OPTIMIZE2 = false;

  int modeInfo[10];

  // Origin, optimize 1, optimize 2
  vector<string> dictionary, sieve, finder;

  // Customized string.find()
  static inline vector<int> findLetter(string S, char letter) {
    vector<int> result;
    for (int i = 0; i < S.size(); ++i)
      if (S[i] == letter) result.push_back(i);
    return result;
  }

  // Word finder
  string generateWord(vector<string> source) {
    auto check = [&](const string &S) {
      for (auto i : S)
        if (findLetter(S, i).size() > 1) return 0;
      return 1;
    };

    int randomCnt = 0;
    string result;

    do {
      ++randomCnt;
      result = source[randNum(0, (int)source.size() - 1)];
    } while (!check(result) && randomCnt < source.size());

    return result;
  }

  // Optimize 2
  bool reduceFinder() {
    if (modeInfo[gameTurns - 1] == 1) return 0;

    // Last game used optimize 2, reduce dictionary.
    // TODO: sort instead of rand to get better solution.
    if (modeInfo[gameTurns - 1] == 2) {
      for (int i = 0; i < finder.size();) {
        int appearCnt = 0;
        for (auto j : finder[i])
          for (auto k : lastWord)
            if (j == k) {
              ++appearCnt;
              break;
            }
        if (appearCnt > 1) {
          std::swap(finder[i], finder.back());
          finder.pop_back();
        } else {
          ++i;
        }
      }
      return !finder.empty();
    }

    // Last game wasn't optimized, use optimize 2.
    vector<char> letterToBeCheck;
    for (int i = 0; i < gameWordLength; ++i) {
      if (lastResult[i] == '0')
        for (auto j : dictionary)
          letterToBeCheck.push_back(j[i]);
    }
    std::sort(letterToBeCheck.begin(), letterToBeCheck.end());
    letterToBeCheck.erase(
        std::unique(letterToBeCheck.begin(), letterToBeCheck.end()),
        letterToBeCheck.end());

    vector<string> finderCopy = finder;
    for (int i = 0; i < finder.size();) {
      int appearCnt = 0;
      bool isCnted[26] = {0};
      for (auto j : finder[i])
        for (auto k : letterToBeCheck)
          if (j == k && !isCnted[k - 'a']) ++appearCnt, isCnted[k - 'a'] = 1;
      if (appearCnt < gameWordLength - 1 || finder[i] == lastWord) {
        std::swap(finder[i], finder.back());
        finder.pop_back();
      } else {
        ++i;
      }
    }

    if (finder.empty() && letterToBeCheck.size() < 7) {
      for (int i = 0; i < finderCopy.size();) {
        int appearCnt = 0;
        bool isCnted[26] = {0};
        for (auto j : finderCopy[i])
          for (auto k : letterToBeCheck)
            if (j == k && !isCnted[k - 'a']) ++appearCnt, isCnted[k - 'a'] = 1;
        if (appearCnt < gameWordLength - 2 || finderCopy[i] == lastWord) {
          std::swap(finderCopy[i], finderCopy.back());
          finderCopy.pop_back();
        } else {
          ++i;
        }
      }
      finder = finderCopy;
    }

    return !finder.empty();
  }

  void checkNonExist(char letter) {
    if (ENABLE_DETAILS) cout << "Checking: " << letter << " does not exist.\n";

    for (int i = 0; i < dictionary.size();) {
      auto result = findLetter(dictionary[i], letter);
      if (!result.empty()) {
        swap(dictionary[i], dictionary.back());
        dictionary.pop_back();
      } else {
        ++i;
      }
    }

    if (ENABLE_OPTIMIZE) {
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

  void checkConfirmed(int pos, char letter) {
    if (ENABLE_DETAILS)
      cout << "Checking: " << letter << " at " << pos + 1 << ".\n";

    for (int i = 0; i < dictionary.size();) {
      if (dictionary[i][pos] != letter) {
        swap(dictionary[i], dictionary.back());
        dictionary.pop_back();
      } else {
        ++i;
      }
    }

    if (ENABLE_OPTIMIZE) {
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

  void checkAppear(vector<int> wrongPos, char letter, int appearTimes) {
    if (ENABLE_DETAILS)
      cout << "Checking: " << letter << " appears " << appearTimes
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

    for (int i = 0; i < sieve.size();) {
      auto result = findLetter(sieve[i], letter);
      bool illegal = 0;
      for (auto j : wrongPos)
        if (sieve[i][j] == letter) {
          illegal = 1;
          break;
        }
      if (illegal) {
        swap(sieve[i], sieve.back());
        sieve.pop_back();
      } else {
        ++i;
      }
    }
  }

  void checkMoreThan(char letter, int appearTimes) {
    if (ENABLE_DETAILS)
      cout << "Checking: " << letter << " appears less than " << appearTimes
           << " times.\n";

    for (int i = 0; i < dictionary.size();) {
      auto result = findLetter(dictionary[i], letter);
      if (result.size() > appearTimes) {
        swap(dictionary[i], dictionary.back());
        dictionary.pop_back();
      } else {
        ++i;
      }
    }

    if (ENABLE_OPTIMIZE) {
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

#define USE_BEST_STARTING_WORD

 protected:
  const int MAX_GAME_TURNS = 6;

  int gameTurns = 0, gameWordLength;
  string winIdentifier, lastWord, lastResult;

  // Initialize a game.
  void init(bool showDetail = false, bool useOptimizeSolution = false) {
    if (showDetail) ENABLE_DETAILS = true;
    if (useOptimizeSolution) ENABLE_OPTIMIZE = true;

    cout << "* Wordle on! Enter the length of your word.\n";
    while (1) {
      cin >> gameWordLength;
      if (gameWordLength < MIN_WORD_LENGTH || gameWordLength > MAX_WORD_LENGTH)
        cout << "* Error! Length should be on [4,11].\n";
      else
        break;
    }
    for (int i = 1; i <= gameWordLength; ++i) winIdentifier += '1';

    // Initialize dictionary, source
    // @https://github.com/lorenbrichter/Words/blob/master/Words/en.txt
    std::ifstream fin;
    string inputWords;
    string requireDic = {"0.txt"};
    requireDic[0] = gameWordLength - 4 + '0';
    fin.open(requireDic);
    while (fin >> inputWords) dictionary.push_back(inputWords);
    fin.close();

    sieve = dictionary;
    finder = dictionary;

    for (int i = 0; i < sieve.size();) {
      bool illegal = 0;
      for (auto j : sieve[i])
        if (findLetter(sieve[i], j).size() > 1) {
          illegal = 1;
          break;
        }
      if (illegal) {
        std::swap(sieve[i], sieve.back());
        sieve.pop_back();
      } else {
        ++i;
      }
    }

    return;
  }

  // Generate a random word from the current dictionary.
  void guess() {
    if (dictionary.empty()) {
      cout << "* Oops! No matching words found. Plz add a issue.\n";
      exit(-1);
    }

#ifdef USE_BEST_STARTING_WORD
    if (gameTurns == 1) {
      // Make the first guess. You can modify the "best" starting word
      // as your idea.
      if (ENABLE_DETAILS)
        cout << "* Remaining " << dictionary.size() << " possible words.\n";

      cout << "* Guess: " << BEST[gameWordLength - 4] << "\n";
      lastWord = BEST[gameWordLength - 4];

      return;
    }
#endif

    if (ENABLE_DETAILS) {
      cout << "* Remaining " << dictionary.size() << " possible words:\n";
      for (auto i : dictionary) cout << i << " ";
      cout << "\n";
    }

    bool optimizedFlag = 0;

    if (ENABLE_OPTIMIZE) {
      if (OPTIMIZE2 && dictionary.size() < 100 && dictionary.size() > 2) {
        if (reduceFinder()) {
          cout << "* Guess: " << (lastWord = generateWord(finder)) << "\n";
          modeInfo[gameTurns] = 2;
          optimizedFlag = 1;
        }
      }
      if (!optimizedFlag && OPTIMIZE1 && !sieve.empty() &&
          dictionary.size() > 5) {
        cout << "* Guess: " << (lastWord = generateWord(sieve)) << "\n";
        modeInfo[gameTurns] = 1;
        optimizedFlag = 1;
      }
    }

    if (optimizedFlag) {
      if (ENABLE_DETAILS) cout << "* Optimized.\n";
    } else {
      cout << "* Guess: " << (lastWord = generateWord(dictionary)) << "\n";
    }

    return;
  }

  // Update dictionary by any word and its result.
  void solveGame(string gameWord) {
    std::map<char, vector<int>> wrongPos;
    std::map<char, int> letterCnt;
    vector<char> receivedZero;
    int confirmedCnt = 0, nonCnt = 0;

    for (int i = 0; i < lastResult.size(); ++i) {
      if (lastResult[i] == '0')
        receivedZero.push_back(gameWord[i]), ++nonCnt;
      else if (lastResult[i] == '1')
        checkConfirmed(i, gameWord[i]), ++letterCnt[gameWord[i]],
            ++confirmedCnt;
      else
        wrongPos[gameWord[i]].push_back(i), ++letterCnt[gameWord[i]];
    }

    for (auto i : receivedZero)
      if (letterCnt[i] == 0)
        checkNonExist(i);
      else
        checkMoreThan(i, letterCnt[i]);  // According to Wordle's rule

    for (auto i : wrongPos)
      if (i.second.size() > 0)
        checkAppear(i.second, i.first, letterCnt[i.first]);

    // Optimize for the next turn
    if (ENABLE_OPTIMIZE) {
      if (gameTurns >= BEGIN_OPTIMIZE1 && gameTurns <= END_OPTIMIZE1 &&
          nonCnt > gameWordLength * 2 / 3)
        OPTIMIZE1 = true;
      else
        OPTIMIZE1 = false;

      if (confirmedCnt >= (gameWordLength + 1) / 2 || gameTurns > END_OPTIMIZE1)
        OPTIMIZE2 = true, OPTIMIZE1 = false;
      if (gameTurns == MAX_GAME_TURNS - 1) OPTIMIZE2 = false;
    }
  }
};

class newGame : public Wordle {
 public:
  void play(int option) {
    cout << "- Usage: 0 means not exist, 1 means correct, 2 means wrong "
            "position. e.g.\"01201\".\n  Enter a single \"-1\" can close "
            "the game.\n";
    init(option & (1 << 10), option & (1 << 11));

    while (++gameTurns <= MAX_GAME_TURNS) {
      guess();

      cout << "- Enter the results: ";
      cin >> lastResult;

      if (lastResult == "-1" || lastResult == winIdentifier) break;

      if (lastResult.size() != gameWordLength) {
        cout << "- Illegal input, try again.\n";
        continue;
      }

      bool illegal = 0;
      for (auto i : lastResult)
        if (i != '0' && i != '1' && i != '2') {
          illegal = 1;
          break;
        }
      if (illegal) {
        cout << "- Illegal input, try again.\n";
        continue;
      }

      solveGame(lastWord);
    }

    if (lastResult == "-1" || lastResult == winIdentifier) {
      cout << "- Niiiice!!\n";
    } else {
      cout << "- Unfortunately, this program cannot solve this problem in "
           << MAX_GAME_TURNS
           << " turns, you can try to change the random "
              "seed or provides the answer in issues. The algorithm will be "
              "optimized in the future.\n";
    }
  }
};

// There is no guarantee that you can get correct answer in this mode
class continueGame : public Wordle {
 private:
  int alreadyTurns;
  string userWord;

 public:
  void play(int option) {
    cout << "- Usage: 0 means not exist, 1 means correct, 2 means wrong "
            "position. e.g.\"01201\".\n  Enter a single \"-1\" can close "
            "the game.\n";
    init(option & (1 << 10));

    cout << "- Enter the times you have guessed.\n";
    do {
      cin >> alreadyTurns;
    } while (alreadyTurns < 0 || alreadyTurns >= MAX_GAME_TURNS);

    while (++gameTurns <= alreadyTurns) {
      cout << "- Enter the word you guessed in turn " << gameTurns << ".\n";
      cin >> userWord;

      if (userWord.size() != gameWordLength) {
        cout << "- Illegal input, try again.\n";
        continue;
      }

      cout << "- Enter the results: ";
      cin >> lastResult;

      if (lastResult == "-1" || lastResult == winIdentifier) break;

      if (lastResult.size() != gameWordLength) {
        cout << "- Illegal input, try again.\n";
        continue;
      }

      bool illegal = 0;
      for (auto i : lastResult)
        if (i != '0' && i != '1' && i != '2') {
          illegal = 1;
          break;
        }
      if (illegal) {
        cout << "- Illegal input, try again.\n";
        continue;
      }

      solveGame(userWord);
    }

    cout << "- Data loaded, program start.\n";

    while (++gameTurns <= MAX_GAME_TURNS) {
      guess();

      cout << "- Enter the results: ";
      cin >> lastResult;

      if (lastResult == "-1" || lastResult == winIdentifier) break;

      if (lastResult.size() != gameWordLength) {
        cout << "- Illegal input, try again.\n";
        continue;
      }

      bool illegal = 0;
      for (auto i : lastResult)
        if (i != '0' && i != '1' && i != '2') {
          illegal = 1;
          break;
        }
      if (illegal) {
        cout << "- Illegal input, try again.\n";
        continue;
      }

      solveGame(lastWord);
    }

    if (lastResult == "-1" || lastResult == winIdentifier) {
      cout << "- Niiiice!!\n";
    } else {
      cout << "- Unfortunately, this program cannot solve this problem in "
           << MAX_GAME_TURNS << " turns, you can try to change the random "
              "seed or provides the answer in issues. The algorithm will be "
              "optimized in the future.\n";
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