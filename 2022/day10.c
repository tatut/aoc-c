#include "../aoc.h"

long x = 1, y = 1;
long cycle = 1;

int x_on[221] = {0, 1}; // x value on a specific cycle (0th is ignored)


void draw(bool *px) {

  for (int y_ = 0; y_ < 6; y_++) {
    for (int x_ = 0; x_ < 40; x_++) {
      //printf("px[40*%d+%d] = %d\n", y_, x_, px[40*y_+x_]);
      printf("%c", px[40*y_+x_] ? '#' : '.');
    }
    printf("\n");
  }
  printf("------------\n");
}

#define SET                                                                    \
  if ((cycle%40) >= x && (cycle%40) <= x+2) \
    px[cycle] = 1;

void day10(Day *day) {
  bool px[40*6] = {0};
  str lines = day->input, line;

  draw(px);

  while (str_each_line(&lines, &line)) {
    x_on[cycle] = x;
    SET;
    if (str_eq(line, str_constant("noop"))) {
      cycle += 1;
      SET;
    } else {
      long add;
      if (!str_scan("addx %ld", line, &add))
        goto fail;

      cycle += 1;
      x_on[cycle] = x;
      SET;
      cycle += 1;
      x += add;
      SET;
    }
  }
  PART1(x_on[20]*20 + x_on[60]*60 + x_on[100]*100 + x_on[140]*140 + x_on[180]*180 + x_on[220]*220);

  draw(px);
    return;
  fail:
    fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
    exit(1);
}
