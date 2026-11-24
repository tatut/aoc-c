#include "../aoc.h"
#include <string.h>

#define AT(g,x,y) (g)->data[((g)->w+1)*(y)+(x)]

typedef struct grid {
  char *data;
  int w, h;
} grid;

typedef struct pos {
  int x, y;
} pos;

bool find(grid *g, char item, pos *p) {
  for (int y = 0; y < g->h; y++) {
    for (int x = 0; x < g->w; x++) {
      //printf(" at(%d,%d) = '%c'\n", x, y, AT(g,x,y));
      if (AT(g, x, y) == item) {
        *p = (pos){.x = x, .y = y};
        return true;
      }
    }
  }
  return false;
}

bool can_move(void *gptr, int x1, int y1, int x2, int y2) {
  grid *g = (grid *)gptr;
  char at1 = AT(g, x1, y1), at2 = AT(g, x2, y2);
  if (at1 == 'S') at1='a';
  if (at2 == 'E') at2='z';

  //printf("can move (%d,%d,%c)=>(%d,%d,%c)\n", x1,y1, AT(g,x1,y1), x2, y2, AT(g,x2,y2));
  return (at2 - at1) <= 1;
}
void day12(Day *day) {
  grid g = (grid){.data = day->input.data};
  g.w = str_indexof(day->input, '\n');
  g.h = (day->input.len + 1) / (g.w + 1);

  pos start, end;
  find(&g, 'S', &start);
  find(&g, 'E', &end);

  long p1 = grid4_shortest_distance(&g, g.w, g.h, start.x, start.y, end.x, end.y,
                                    can_move);
  PART1(p1);

  long p2 = p1;
  for (int y = 0; y < g.h; y++) {
    for (int x = 0; x < g.w; x++) {
      if (AT(&g, x, y) == 'a') {
        long l =
            grid4_shortest_distance(&g, g.w, g.h, x, y, end.x, end.y, can_move);
        if(l < p2) p2 = l;
      }
    }
  }
  PART2(p2);

}
