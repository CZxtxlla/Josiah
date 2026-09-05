# Progression
---
Version 0.0.1 is the first functioning version. It implements negamax alpha beta pruning with iterative deepening and basic time management, as well as a purely material based evaluation function.

Version 0.0.2 adds piece square tables to the evaluation. Here is the results of sprt with version 0.0.1:

Score of Josiah v0.0.2 vs Josiah v0.0.1: 78 - 25 - 89  [0.638] 192
...      Josiah v0.0.2 playing White: 36 - 13 - 47  [0.620] 96
...      Josiah v0.0.2 playing Black: 42 - 12 - 42  [0.656] 96
...      White vs Black: 48 - 55 - 89  [0.482] 192
Elo difference: 98.5 +/- 36.2, LOS: 100.0 %, DrawRatio: 46.4 %
SPRT: llr 2.97 (101.0%), lbound -2.94, ubound 2.94 - H1 was accepted

There are a lot of draws due to threefold repetition, which should be solved once I implement the transposition table


Version 0.0.2.1 fixes a major issue with the evaluation . Here is the results of sprt with version 0.0.2:

Score of Josiah v0.0.2.1 vs Josiah v0.0.2: 106 - 51 - 148  [0.590] 305
...      Josiah v0.0.2.1 playing White: 64 - 21 - 67  [0.641] 152
...      Josiah v0.0.2.1 playing Black: 42 - 30 - 81  [0.539] 153
...      White vs Black: 94 - 63 - 148  [0.551] 305
Elo difference: 63.3 +/- 28.0, LOS: 100.0 %, DrawRatio: 48.5 %
SPRT: llr 2.95 (100.2%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.0.3 adds quiescent search. Here is the result of sprt with version 0.0.2.1

Score of Josiah v0.0.3 vs Josiah v0.0.2.1: 60 - 10 - 50  [0.708] 120
...      Josiah v0.0.3 playing White: 33 - 4 - 23  [0.742] 60
...      Josiah v0.0.3 playing Black: 27 - 6 - 27  [0.675] 60
...      White vs Black: 39 - 31 - 50  [0.533] 120
Elo difference: 154.1 +/- 48.6, LOS: 100.0 %, DrawRatio: 41.7 %
SPRT: llr 2.97 (100.8%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.0.4 adds very basic MVV-LVA move ordering. Here is the result of sprt with version 0.0.3:

Score of Josiah v0.0.4 vs Josiah v0.0.3: 112 - 60 - 196  [0.571] 368
...      Josiah v0.0.4 playing White: 58 - 25 - 101  [0.590] 184
...      Josiah v0.0.4 playing Black: 54 - 35 - 95  [0.552] 184
...      White vs Black: 93 - 79 - 196  [0.519] 368
Elo difference: 49.4 +/- 24.2, LOS: 100.0 %, DrawRatio: 53.3 %
SPRT: llr 2.97 (100.9%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.0.5 adds a transposition table. Here is the result of sprt with version 0.0.4:

Score of Josiah v0.0.5 vs Josiah v0.0.4: 147 - 96 - 341  [0.544] 584
...      Josiah v0.0.5 playing White: 89 - 37 - 167  [0.589] 293
...      Josiah v0.0.5 playing Black: 58 - 59 - 174  [0.498] 291
...      White vs Black: 148 - 95 - 341  [0.545] 584
Elo difference: 30.4 +/- 18.1, LOS: 99.9 %, DrawRatio: 58.4 %
SPRT: llr 2.99 (101.6%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.0.6 adds repetition checking. Here is the result of sprt with version 0.0.5:

Score of Josiah v0.0.6 vs Josiah v0.0.5: 115 - 59 - 166  [0.582] 340
...      Josiah v0.0.6 playing White: 73 - 26 - 71  [0.638] 170
...      Josiah v0.0.6 playing Black: 42 - 33 - 95  [0.526] 170
...      White vs Black: 106 - 68 - 166  [0.556] 340
Elo difference: 57.8 +/- 26.4, LOS: 100.0 %, DrawRatio: 48.8 %
SPRT: llr 2.98 (101.1%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.0.6.1 adds hash moves to move ordering. Here is the result of sprt with version 0.0.6:

Score of Josiah v0.0.6.1 vs Josiah v0.0.6: 129 - 51 - 59  [0.663] 239
...      Josiah v0.0.6.1 playing White: 72 - 17 - 31  [0.729] 120
...      Josiah v0.0.6.1 playing Black: 57 - 34 - 28  [0.597] 119
...      White vs Black: 106 - 74 - 59  [0.567] 239
Elo difference: 117.7 +/- 39.8, LOS: 100.0 %, DrawRatio: 24.7 %
SPRT: llr 2.98 (101.2%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.0.6.2 adds new compilation flags. Here is the result of sprt with version 0.0.6.1:

Score of Josiah v0.0.6.2 vs Josiah v0.0.6.1: 321 - 234 - 188  [0.559] 743
...      Josiah v0.0.6.2 playing White: 188 - 94 - 91  [0.626] 373
...      Josiah v0.0.6.2 playing Black: 133 - 140 - 97  [0.491] 370
...      White vs Black: 328 - 227 - 188  [0.568] 743
Elo difference: 40.9 +/- 21.7, LOS: 100.0 %, DrawRatio: 25.3 %
SPRT: llr 2.96 (100.4%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.0.7 swaps legal move generation for pseudolegal in negamax. As of this version I am switching testing to a hash size of 64 MB from 256 MB. Here is the result of sprt with version 0.0.6.2 (with hash size 64):

Score of Josiah v0.0.7 vs Josiah v0.0.6.2_64: 682 - 581 - 439  [0.530] 1702
...      Josiah v0.0.7 playing White: 394 - 249 - 208  [0.585] 851
...      Josiah v0.0.7 playing Black: 288 - 332 - 231  [0.474] 851
...      White vs Black: 726 - 537 - 439  [0.556] 1702
Elo difference: 20.6 +/- 14.2, LOS: 99.8 %, DrawRatio: 25.8 %
SPRT: llr 2.97 (100.9%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.0.8 adds PVS. Here is the result of sprt with version 0.0.7:

Score of Josiah v0.0.8 vs Josiah v0.0.7: 296 - 209 - 167  [0.565] 672
...      Josiah v0.0.8 playing White: 186 - 76 - 75  [0.663] 337
...      Josiah v0.0.8 playing Black: 110 - 133 - 92  [0.466] 335
...      White vs Black: 319 - 186 - 167  [0.599] 672
Elo difference: 45.2 +/- 22.9, LOS: 100.0 %, DrawRatio: 24.9 %
SPRT: llr 2.98 (101.2%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.0.9 adds RFP. Here is the result of sprt with version 0.0.8:

Score of Josiah v0.0.9 vs Josiah v0.0.8: 283 - 195 - 138  [0.571] 616
...      Josiah v0.0.9 playing White: 173 - 68 - 68  [0.670] 309
...      Josiah v0.0.9 playing Black: 110 - 127 - 70  [0.472] 307
...      White vs Black: 300 - 178 - 138  [0.599] 616
Elo difference: 50.0 +/- 24.4, LOS: 100.0 %, DrawRatio: 22.4 %
SPRT: llr 2.96 (100.4%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.0.9.1 adds NMP. Here is the result of sprt with version 0.0.9:

Score of Josiah v0.0.9.1 vs Josiah v0.0.9: 106 - 32 - 47  [0.700] 185
...      Josiah v0.0.9.1 playing White: 57 - 12 - 25  [0.739] 94
...      Josiah v0.0.9.1 playing Black: 49 - 20 - 22  [0.659] 91
...      White vs Black: 77 - 61 - 47  [0.543] 185
Elo difference: 147.2 +/- 46.0, LOS: 100.0 %, DrawRatio: 25.4 %
SPRT: llr 2.97 (100.8%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.0.9.2 adds basic LMR reducing if quiet and by 1 if less than 6 moves played and by two otherwise. Here is the result of sprt with version 0.0.9.1:

Score of Josiah v0.0.9.2 vs Josiah v0.0.9.1: 409 - 320 - 272  [0.544] 1001
...      Josiah v0.0.9.2 playing White: 244 - 125 - 132  [0.619] 501
...      Josiah v0.0.9.2 playing Black: 165 - 195 - 140  [0.470] 500
...      White vs Black: 439 - 290 - 272  [0.574] 1001
Elo difference: 31.0 +/- 18.4, LOS: 100.0 %, DrawRatio: 27.2 %
SPRT: llr 2.96 (100.5%), lbound -2.94, ubound 2.94 - H1 was accepted

Version 0.1.0 adds the history heuristic to move ordering. Here is the result of sprt with version 0.0.9.2:

Score of Josiah v0.1.0 vs Josiah v0.0.9.2: 190 - 110 - 109  [0.598] 409
...      Josiah v0.1.0 playing White: 112 - 45 - 47  [0.664] 204
...      Josiah v0.1.0 playing Black: 78 - 65 - 62  [0.532] 205
...      White vs Black: 177 - 123 - 109  [0.566] 409
Elo difference: 68.8 +/- 29.2, LOS: 100.0 %, DrawRatio: 26.7 %
SPRT: llr 2.96 (100.4%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.1.1 adds the killer moves heuristic. Here is the result of sprt with version 0.1.0:

Score of Josiah v0.1.1 vs Josiah v0.1.0: 280 - 197 - 186  [0.563] 663
...      Josiah v0.1.1 playing White: 164 - 73 - 95  [0.637] 332
...      Josiah v0.1.1 playing Black: 116 - 124 - 91  [0.488] 331
...      White vs Black: 288 - 189 - 186  [0.575] 663
Elo difference: 43.7 +/- 22.5, LOS: 100.0 %, DrawRatio: 28.1 %
SPRT: llr 2.96 (100.5%), lbound -2.94, ubound 2.94 - H1 was accepted


Version 0.1.2 adds promotions to move ordering and promotion restriction on history/killer moves. Here is the result of sprt with version 0.1.1:

Score of Josiah v0.1.2 vs Josiah v0.1.1: 3147 - 3035 - 2521  [0.506] 8703
...      Josiah v0.1.2 playing White: 1913 - 1156 - 1283  [0.587] 4352
...      Josiah v0.1.2 playing Black: 1234 - 1879 - 1238  [0.426] 4351
...      White vs Black: 3792 - 2390 - 2521  [0.581] 8703
Elo difference: 4.5 +/- 6.1, LOS: 92.3 %, DrawRatio: 29.0 %
SPRT: llr -0.537 (-18.2%), lbound -2.94, ubound 2.94

Note that I stopped it early (after 8703 games), it definitely improved but only in extreme edge cases where promotions even come up and the game isn't already over.


Version 0.2.0 adds basic staged movegen. Here is the result of sprt with version 0.1.3:

Score of Josiah v0.2.0 vs Josiah v0.1.3: 1262 - 1140 - 931  [0.518] 3333
...      Josiah v0.2.0 playing White: 782 - 439 - 445  [0.603] 1666
...      Josiah v0.2.0 playing Black: 480 - 701 - 486  [0.434] 1667
...      White vs Black: 1483 - 919 - 931  [0.585] 3333
Elo difference: 12.7 +/- 10.0, LOS: 99.4 %, DrawRatio: 27.9 %
SPRT: llr 2.96 (100.5%), lbound -2.94, ubound 2.94 - H1 was accepted



### Future Improvements

These are just some performance enhancements that I noticed in other engines that I would like to implement eventually.
- Lazy move ordering (proper movepicker (not what I have now))
- PEXT instruction magic bitboards.
- Bucketed TT
- IIR https://talkchess.com/forum3/viewtopic.php?f=7&t=74769&sid=64085e3396554f0fba414404445b3120
- Persistant history table across the whole game


### Things I have tried that haven't panned out (some might be worth revisiting later)
- not reducing if the opponents king is in check
- 