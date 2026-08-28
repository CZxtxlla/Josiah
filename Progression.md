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

