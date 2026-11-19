#include "../aoc.h"

typedef struct grid {
  int w, h;
  char *data;
} grid;

grid str_to_grid(str input) {
  int w = 0;
  while (str_char_at(input, w) != '\n') {
    w++;
  }
  int h = input.len / (w + 1);

  return (grid){.w = w, .h = h, .data = input.data};
}

#define AT(g, x, y) ((g).data[(y) * ((g).w + 1) + (x)])

bool visible(grid g, int x, int y) {
  char hg = AT(g, x, y);
  if(x == 0 || y == 0 || x == g.w-1 || y == g.h-1) return true;
  int vis = 4;
  for (int i = x-1; i >= 0; i--) {
    if (AT(g, i, y) >= hg) {
      vis--;
      break;
    }
  }
  for (int i = x+1; i < g.w; i++) {
    if (AT(g, i, y) >= hg) {
      vis--;
      break;
    }
  }
  for (int i = y-1; i >= 0; i--) {
    if (AT(g, x, i) >= hg) {
      vis--;
      break;
    }
  }
  for (int i = y + 1; i < g.h; i++) {
    if (AT(g, x, i) >= hg) {
      vis--;
      break;
    }
  }
  return vis > 0;
}

long scenic_score(grid g, int x, int y) {
  if (x == 0 || y == 0 || x == g.w - 1 || y == g.h - 1)
    return 0;
  long l=0,r=0,u=0,d=0;
  char hg = AT(g,x,y);
  for (int i = x - 1; i >= 0; i--) {
    char at = AT(g, i, y);
    if(at >= hg) {
      l++;
      break;
    } else {
      l++;
    }
  }
  for (int i = x+1; i < g.w; i++) {
    char at = AT(g, i, y);
    if (at >= hg) {
      r++;
      break;
    } else {
      r++;
    }
  }
  for (int i = y-1; i >= 0; i--) {
    char at = AT(g, x, i);
    if(at >= hg) {
      u++;
      break;
    } else {
      u++;
    }
  }
  for (int i = y + 1; i < g.h; i++) {
    char at = AT(g, x, i);
    if(at >= hg) {
      d++;
      break;
    } else {
      d++;
    }
  }

  return l*r*u*d;
}

void day8(Day *day) {
  grid g = str_to_grid(day->input);
  long total_vis = 0;
  for (int x = 0; x < g.w; x++) {
    for (int y = 0; y < g.h; y++) {
      total_vis += visible(g, x,y);
    }
  }
  PART1(total_vis);

  long best_scenic_score = 0;
  for (int x = 1; x < g.w - 1; x++) {
    for (int y = 1; y < g.h - 1; y++) {
      long s = scenic_score(g, x, y);
      if (s > best_scenic_score)
        best_scenic_score = s;
    }
  }
  PART2(best_scenic_score);
}
