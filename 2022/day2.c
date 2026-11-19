#include "../aoc.h"

// round is 4 bits (high two bits is me, low two bits is opponent)
// 01 = rock, 10 = paper, 11 = scissor
// winner: 0 = tie, 1 = me, 2 = opponent
uint8_t win[] = {
  [0b0101] = 0, //    rock vs rock
  [0b0110] = 2, //    rock vs paper
  [0b0111] = 1, //    rock vs scissor
  [0b1001] = 1, //   paper vs rock
  [0b1010] = 0, //   paper vs paper
  [0b1011] = 2, //   paper vs scissor
  [0b1101] = 2, // scissor vs rock
  [0b1110] = 1, // scissor vs paper
  [0b1111] = 0  // scissor vs scissor
};

// what to play to win or lose against
uint8_t to_win[] = {[0b01] = 0b10, [0b10] = 0b11, [0b11] = 0b01};
uint8_t to_lose[] = {[0b01] = 0b11, [0b10] = 0b01, [0b11] = 0b10};

int move_score(char me, char op) {
  int score = me;
  switch (win[(me << 2) + op]) {
  case 0:
    score += 3; // 3 points for tie
    break;
  case 1:
    score += 6; // 6 points if I win
    break;
  case 2:
    score += 0; // no points if opponent wins
    break;
  }
  return score;
}

void day2(Day *day) {
  str line, lines;
  lines = day->input;
  int score = 0;
  int score2 = 0;
  while (str_each_line(&lines, &line)) {
    // opponent move: ABC, my move: XYZ
    char op = 1 + (str_char_at(line, 0) - 'A');
    char me = 1 + (str_char_at(line, 2) - 'X');
    score += move_score(me, op);

    // part2 X means I must lose, Y means tie and Z means I must win
    switch (me) {
    case 1: // X mean I must lose
      me = to_lose[op];
      break;
    case 2: // Y means tie
      me = op;
      break;
    case 3: // Z means I must win
      me = to_win[op];
      break;
    }
    score2 += move_score(me, op);

  }
  PART1(score);
  PART2(score2);
}
