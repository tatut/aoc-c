#include "../aoc.h"

#define SZ 64
char grid[SZ*SZ];

#define IDX(x, y) (((y) * SZ) + (x))
#define POS(x, y, steps) (((steps) << 12) + ((y) << 6) + x)
#define STEPS(pos) ((pos)>>12)
#define X(pos) ((pos) & 0b111111)
#define Y(pos) (((pos)>>6) & 0b111111)

long popcount(uint64_t n) {
  uint64_t orig = n;
  long count = 0;
  while (n) {
    n &= (n - 1);
    count++;
  }
  return count;
}


void fill(long in) {
  for(int y=0;y<SZ;y++) {
    for(int x=0;x<SZ;x++) {
      uint64_t bits = in + (x*x + 3*x + 2*x*y + y + y*y);
      grid[IDX(x,y)] = (popcount(bits) % 2 == 0) ? '.' : '#';
      printf("%c",grid[y*SZ+x]);
    }
    printf("\n");
  }
}

int reach(int tx, int ty) {
  bool visited[SZ*SZ] = {0};
  queue q = initq(int);
  int start = POS(1, 1, 0);
  enq(q, start);

  int min_steps = 999999;
  while (q.size) {
    int n = deq(q, int);
    int x = X(n);
    int y = Y(n);
    int s = STEPS(n);
    if (x == tx && y == ty) {
      if (s < min_steps) {
        min_steps = s;
      }
    }
    if (s >= min_steps) // already found a better or as good solution
      continue;

    // enqueue possible neighbors
    for (int ny = y - 1; ny <= y + 1; ny++) {
      if(ny < 0 || ny >= SZ) continue;
      for (int nx = x - 1; nx <= x + 1; nx++) {
        if(nx < 0 || nx >= SZ) continue;
        if (nx != x && ny != y)
          continue; // no diagonal neighbors
        if(grid[IDX(nx,ny)] == '#') continue;
        int n = POS(nx, ny, s + 1);
        if (!visited[IDX(nx, ny)]) {
          visited[IDX(nx,ny)] = 1;
          enq(q, n);
        }
      }
    }
  }
  freeq(&q);
  return min_steps;
}

int reachable(int in_steps) {
  bool visited[SZ*SZ] = {0};
  queue q = initq(int);
  int start = POS(1, 1, 0);
  enq(q, start);

  visited[IDX(1,1)] = 1;
  while (q.size) {
    int n = deq(q, int);
    int x = X(n);
    int y = Y(n);
    int s = STEPS(n);

    if (s == in_steps) continue;

    // enqueue possible neighbors
    for (int ny = y - 1; ny <= y + 1; ny++) {
      if(ny < 0 || ny >= SZ) continue;
      for (int nx = x - 1; nx <= x + 1; nx++) {
        if(nx < 0 || nx >= SZ) continue;
        if (nx != x && ny != y)
          continue; // no diagonal neighbors
        if(grid[IDX(nx,ny)] == '#') continue;
        int n = POS(nx, ny, s + 1);
        if (!visited[IDX(nx, ny)]) {
          visited[IDX(nx,ny)] = 1;
          enq(q, n);
        }
      }
    }
  }

  int count = 0;
  for(int i=0;i<SZ*SZ;i++) count += visited[i];
  return count;
}



void day13(Day *day){
  long in = str_to_long(str_trim(day->input));
  fill(in);
  PART1(reach(31, 39)); // 82
  PART2(reachable(50)); // 138
}
