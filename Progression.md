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


Version 0.0.5 is planned to add transposition table.