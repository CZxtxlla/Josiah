# Josiah II
---

Josiah II is a work in progress UCI chess engine. It is my second major attempt at a uci compatible chess engine written in C.
The objective of Josiah II is to write a chess engine using zero llm input. Josiah, while a competitive and working engine, suffered at the hands of gemini, and I found myself losing touch with what I was actually writing. 
Now, instead of relying on llms I will be taking inspiration from other engines and especially the chess programming wiki. None of the code in the project will be ai generated whatsoever.

Additionally, from the start I will be optimizing this for my ryzen 5000 series processor, which means I will be using AVX2 and no AVX-512 (I might add support for that later but it's not a priority).

My main references are the engines Ethereal and Berserk. (My movegen code is heavily inspired/sourced from Berserk, as well as the nice video series Berserk was based upon:
https://www.youtube.com/channel/UCB9-prLkPwgvlKKqDgXhsMQ/videos)

Resources I have used:
https://analog-hors.github.io/site/magic-bitboards/
Chess Programming Wiki