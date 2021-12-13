# Blotto
Multiplayer implementation of Blotto game. Last Updated: 11/07/2019.

## Introduction
Blotto is a simultaneous game for two players in which the players distribute military units over a number of battlefields. 
Each player has a set number of units to distribute, each unit is equivalent to each other unit, and a unit must be allocated entirely to a single battlefield.
Each battlefield is worth a given number of points based on its strategic value, and the player who allocates more units to a battlefield wins the points for that battlefield (with the points split evenly in case of a tie, including a 0-0 tie).
For example, suppose there are four battlefields worth 1, 2, 3, and 4 points respectively and each player has 10 units to distribute. If player 1's distribution is (2,2,2,4) and player 2's distribution is (3,4,2,1) then player 1 wins battlefield 4, player 2 wins battlefields 1 and 2, and the players tie on battlefield 3. 
Player 1's score is then 5.5 and player 2's score is 4.5.

The winner of a single one-on-one competition is the player with the most points in that competition; the competition is a draw (12 win each) if the score is tied.

A variant of Blotto allows for multiple players. Each player submits a single entry, and the entries are played against each other two at a time. The objective can be to accumulate the most wins or the most total points. For example, if we have the two entries above and a third entry (2,2,6,0) then P1 beats P2 5.5-4.5, P1 beats P3 5.5-4.5, and P2 beats P3 7-3. If the overall champion is determined by number of wins, then P1 is the champion with 2 wins. If the overall champion is determined by total score then the champion is P2 with 11.5.

## `smap` ADT
The smap ADT, defines a map from strings as keys to void* values.

## Blotto program
I used the `smap` ADT implementation to write a program that displays the results for a Blotto competition. Competition entries are read from standard input.
Which entries to play against which other entries are determined by a file whose name is given as a command-line argument.

## Input
The command-line arguments are as follows.
* The first command-line argument will be the name of the matchups file.
* The second command-line argument will be either "score" or "win" to determine whether the program keeps track of wins for each entry or the total score for each entry.
* The remaining command-line arguments (at least one) determine the values of each battlefield, which should be given as positive integers in a format readable by `atoi`. The number of battlefields is determined by the number of values.

Standard input should contain one entry per line, where each entry contains a unique non-empty entry id (a string of at most 31 characters containing no commas or whitespace) and the distribution of units given as nonnegative integers in a format readable by `atoi`. 
There should be one integer per battlefield, and the sum of the integers for each entry should be the same. 
All parts of an entry should be separated by commas, with no whitespace except for the newline at the end of the line.

The matchups input file should contain one matchup per line, where each matchup is specified by giving the id of two different entries, separated by a single space, with no other whitespace except for the newline at the end of the line.

## Output
Output gives the average wins or score per game for each entry sorted in decreasing order, with entries that were not present in the matchups file excluded. 
Ties are broken in order of increasing entry id as determined by `strcmp`.


##  Examples
```
$ ./Blotto /c/cs223/hw4/Tests/round_robin_3.in win 1 2 3 4 <  /c/cs223/hw4/Tests/example_3.in
  1.000 P1
  0.500 P2
  0.000 P3
```
```
$ ./Blotto /c/cs223/hw4/Tests/round_robin_3.in score 1 2 3 4 < /c/cs223/hw4/Tests/example_3.in
  5.750 P2
  5.500 P1
  3.750 P3
```
```
$ ./Blotto /c/cs223/hw4/Tests/round_robin_25_5.in win 1 2 3 4 5 < /c/cs223/hw4/Tests/blotto_25_5.in
  0.789 entry15
  0.763 entry23
  0.750 entry20
  0.724 entry32
  0.697 entry1
  0.697 entry37
  0.645 entry11
  0.645 entry22
  0.645 entry35
  0.618 entry34
  0.605 entry29
  0.592 entry14
  0.579 entry12
  0.579 entry3
  0.526 entry28
  0.526 entry7
  0.513 entry39
  0.500 entry21
  0.500 entry24
  0.487 entry6
  0.474 entry4
  0.474 entry9
  0.461 entry10
  0.461 entry16
  0.461 entry17
  0.461 entry19
  0.461 entry25
  0.461 entry8
  0.447 entry13
  0.447 entry33
  0.434 entry18
  0.421 entry38
  0.408 entry26
  0.355 entry2
  0.303 entry30
  0.250 entry5
  0.211 entry31
  0.132 entry36
  0.000 entry27
  ```
