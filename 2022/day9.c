#include "../aoc.h"
#include <stdint.h>

#define SZ 1000
#define START 500

#define BS_SIZE ((SZ * SZ) / 64)

uint64_t data[BS_SIZE] = {0};

// use a bitset of 1000x1000, with start pos center

#define VISITED(x, y)                                                          \
  (data[(((y) * SZ) + (x)) / 64] & ((uint64_t)1 << ((((y) * SZ) + (x)) % 64)))

#define VISIT(x, y)                                                            \
  (data[(((y) * SZ) + (x)) / 64] |= ((uint64_t)1 << ((((y) * SZ) + (x)) % 64)))

#define CLEAR() bzero(data, BS_SIZE*sizeof(uint64_t))

void follow(int hx, int hy, int *tx, int *ty) {
  int dist = abs(hx - *tx) + abs(hy - *ty);
  if(dist < 2) return;
  if (hx == *tx) {
    // same col
    if (hy - *ty > 1)
      *ty += 1;
    else if (hy - *ty < -1)
      *ty -= 1;
  } else if (hy == *ty) {
    // same row
    if (hx - *tx > 1)
      *tx += 1;
    else if (hx - *tx < -1)
      *tx -= 1;
  } else if (dist > 2) {
    // not same row or column and dist greater than 2
    // we need to move tail diagonally
    *tx += (hx < *tx ? -1 : 1);
    *ty += (hy < *ty ? -1 : 1);
  }
}


void day9(Day *day) {
  str lines = day->input, line;
  int hx = START, hy = START, tx = START, ty = START;
  VISIT(tx,ty); // visit start position
  while (str_each_line(&lines, &line)) {
    char dir;
    long count;
    if (!str_scan("%c %ld", line, &dir, &count))
      goto fail;
    int dx=0,dy=0;
    switch (dir) {
    case 'U': dy = -1; break;
    case 'D': dy = 1; break;
    case 'L': dx = -1; break;
    case 'R': dx = 1; break;
    }
    for (int i = 0; i < count; i++) {
      hx += dx;
      hy += dy;
      follow(hx, hy, &tx, &ty);
      VISIT(tx,ty);
    }
  }

  long visited = 0;
  for (int i = 0; i < BS_SIZE; i++)
    visited += popcount(data[i]);
  PART1(visited);





  return;
fail:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
}
