# Josiah
---

Josiah is a work in progress UCI chess engine. It is my second major attempt at a uci compatible chess engine written in C.
The objective of Josiah is to write a chess engine using zero llm input. My original engine suffered at the hands of gemini and I found myself losing touch with what I was writing. 

Additionally, from the start I will be optimizing this for my ryzen 5000 series processor, which means I will be using AVX2 and no AVX-512 (I might add support for that later but it's not a priority).

My main references are the engines Ethereal and Berserk. (My movegen code is heavily inspired/sourced from Berserk, as well as the nice video series Berserk was based upon:
https://www.youtube.com/channel/UCB9-prLkPwgvlKKqDgXhsMQ/videos)

Resources I have used:
- Chess Programming Wiki
- https://analog-hors.github.io/site/magic-bitboards/
- https://chessprogramming.org/Search_Progression (using this as a kind of roadmap to progress through)
- https://talkchess.com/viewtopic.php?t=41302&start=1
- https://chess.stackexchange.com/questions/15856/implementing-late-move-reduction-lmr-inside-iterative-deepening