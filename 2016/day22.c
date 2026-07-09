#include "../aoc.h"

#define W 38
#define H 26
#define MAX W * H


typedef struct Node {
  u16 size; // size = 0, if this node doesn't exist
  u16 used;
} Node;

u16 initial_size[W * H] = {0};
u16 initial_used[W * H] = {0};

#define IDX(x, y) (((y) * W) + (x))

void parse(str lines) {
  str line;
  while (str_each_line(&lines, &line)) {
    if (line.data[0] != '/')
      continue;
    long x,y,size,used;
    if (!str_scan("/dev/grid/node-x%ld-y%ld%w%ldT%w%ldT%&", line, &x, &y, &size,
                  &used))
      goto badline;
    initial_size[IDX(x, y)] = size;
    initial_used[IDX(x,y)] = used;
  }

  return;
badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
  exit(1);
}

long viable() {
  long viable = 0;
  for (int a = 0; a < MAX; a++) {
    if (!initial_size[a])
      continue;
    for (int b = a + 1; b < MAX; b++) {
      if (!initial_size[b])
        continue;
      if (initial_used[a] <= (initial_size[b] - initial_used[b]))
        viable++;
    }
  }
  return viable;
}

/* part2
 * we need to find where the empty disk is, then move that to
 * the target data...
 * then use 5 steps each time to move it 1 spot left
 *
 * turn nodes into 2d grid where # is unpassable, then use BFS
 *
 */
void print_grid(char *g) {
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      printf("%c", g[IDX(x,y)]);
    }
    printf("\n");
  }
}

/* move hole to */

bool can_move(void *grid, int _fromx, int _fromy, int x, int y) {
  printf(" can move to %d, %d => %s\n", x, y,
         (((char*)grid)[IDX(x,y)] != '#') ? "yes" : "no");
  return ((char*)grid)[IDX(x,y)] != '#';
}

long move_hole(char *grid, int sx, int sy, int ex, int ey) {
  return grid4_shortest_distance(grid, W, H, sx, sy, ex, ey, can_move);
}


long part2() {
  char grid[W * H] = {0};
  int empty = 0;
  int ex, ey;
  int dx, dy=0;
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      int i = IDX(x, y);
      if(initial_size[i] && y == 0) dx = x;
      if (initial_size[i] && !initial_used[i]) {
        printf("empty disk at: x:%d, y:%d\n", x,y);
        empty = i;
        ex = x;
        ey = y;
      }
    }
  }
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      if (x == dx && y == dy) {
        grid[IDX(x,y)] = 'G';
      } else if (x == ex && y == ey) {
        grid[IDX(x,y)] = '0';
      } else if (initial_size[IDX(x, y)] > initial_size[empty]) {
        grid[IDX(x, y)] = '#';
      } else {
        grid[IDX(x, y)] = '.';
      }
    }
  }

  print_grid(grid);
  // move hole to left side of target
  long steps = move_hole(grid, ex, ey, dx - 1, dy);
  steps++; // swap with target
  dx--;

  // rest is 5 * dx (to go around it and replace it)
  steps += 5 * dx;
  return steps;
}

void day22(Day *day) {
  parse(day->input);
  PART1(viable());
  PART2(part2());
}
