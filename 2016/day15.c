#include "../aoc.h"

typedef struct Disc {
  uint8_t positions; // how many positions
  uint8_t initial;   // position at t=0
} Disc;

Disc disc[7] = {
  [6] = {.positions = 11, .initial = 0} // part 2 disc
};


inline uint8_t pos_at(Disc *d, int t) {
  return (d->initial + t) % d->positions;
}

void parse(str lines) {
  str line;
  while(str_each_line(&lines, &line)) {
    long idx, positions, initial;
    if(!str_scan("Disc #%ld has %ld positions; at time=0, it is at position %ld.",
                 line, &idx, &positions, &initial)) goto badline;
    disc[idx-1] = (Disc){.positions = positions, .initial = initial};
    printf(" disc %ld, positions: %ld, initial: %ld\n", idx, positions, initial);
  }
  return;
 badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
}

long all_at_zero(int discs) {
  int t=0;
  while(true) {
    bool valid = true;
    for(int i=0;i<discs;i++) {
      if(pos_at(&disc[i], t+i+1)) {
        valid = false;
        break;
      }
    }
    if(valid) return t;
    t++;
  }
}

void day15(Day *day) {
  parse(day->input);
  PART1(all_at_zero(6));
  PART2(all_at_zero(7));
}
