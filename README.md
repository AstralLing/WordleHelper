#### WordleHelper

**中文** | [English](./README_en.md)

这是一个能够自动解决 Wordle 问题的小工具~

相比朴素地筛去不合法的单词，它有着更高效的算法，可以有效提高成功解决 Wordle 的概率与效率。

使用 g++ 自行构建的参考编译选项：
```
g++ wlh.h main.cpp wordle.cpp -o <BinaryFileName> -std=c++17 -O2 -static
```
 
你可以选用此 repo 中提供的词库或者你自己的词库，把它们放在程序的相同目录下并依次命名为 `<[0,7]>.txt` 即可；这可以支持长度为 `[4,11]` 的单词，你也可以根据实际需求只使用特定长度单词的词库（此 repo 下的 `sieve.cpp` 可以帮助你从任意词典中分离指定长度的单词），程序不会运行错误。

未来可能会把词库内嵌在程序中。

#### 命令行
```
    -n: 从头开始游戏（这是默认选项，与 -c 冲突）
    -c: 继续一局游戏，需要依次输入此前几次猜词得到的信息。
    -d: 显示程序运行的详细信息。
    -o: 开启优化（这是默认选项）
```
形如 `-W -n -o` 和 `-Won` 的格式都是可以被识别的。

#### 关于
优化算法的目标主要是避开一些非常糟糕的情况，例如已经确定单词的后四个字母是 `irth` 时，朴素算法必须枚举 `birth`，`mirth`，`firth` 等词语，但显然寻找一个同时包含 `b`，`m`，`f` 等字母的单词是更优秀的选择。

[这里](https://wordlegame.org/)和[这里](https://www.nytimes.com/games/wordle/index.html)都免费提供很棒的在线 Wordle 游戏。

此 repo 的 `results` 目录下会每天更新[这里](https://www.nytimes.com/games/wordle/index.html)的 Wordle 解决过程。因为程序使用的默认词库比此网站的词库大得多，它的解题效率可能略慢于预期。如果你想自己玩每日 Wordle 游戏，不要点开它！

GLHF :)
