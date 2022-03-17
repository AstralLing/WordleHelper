#### WordleHelper

[中文](./README.md) | **English**

A simple wordle game helper with better algorithm.

build:
```
g++ wlh.h main.cpp wordle.cpp -o <BinaryFileName> -std=c++17 -O2 -static
```

To let the program recognize dictionary files, putting them(your customized or default in this repo) in the same folder, files named `<[0,7]>.txt`.

#### cli help
```
    -n: start a new game, the program will solve it automatically. (default)
    -c: continue your game, input the information you already got and the program will show advice.
    -d: show details during game.
    -o: use optimized algorithm.
```
Both format like `-W -n -o` and `-Won` can be recognized.

#### About
The optimization avoids some terrible situations, such as when you have to guess a word between 'birth', 'mirth', 'firth' and so on. You can test it by the word 'birth', which is difficult to be guessed out while using the default seed and unoptimized algo.

Hope you are not tired of reading my poor English. [Here](https://wordlegame.org/) and [here](https://www.nytimes.com/games/wordle/index.html) provide nice online wordle game. GLHF!
