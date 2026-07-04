#include "../aoc.h"
#include <stdint.h>
#include <stdio.h>

typedef enum Instr { RECT, ROTATE_ROW, ROTATE_COL } Instr;

typedef struct Ins {
  Instr type;
  uint8_t a, b;
} Ins;

Ins code[256];
int last_code = 0;

void parse(str lines) {
  str line;
  while(str_each_line(&lines, &line)) {
    if(str_startswith(line, str_constant("rect "))) {
      line = str_drop(line, 5);
      long w, h;
      if(!str_scan("%ldx%ld", line, &w, &h)) goto badline;
      code[last_code++] = (Ins){.type = RECT, .a = w, .b = h};
    } else if(str_startswith(line, str_constant("rotate row y="))) {
      long row, by;
      line = str_drop(line, 13);
      if (!str_scan("%ld by %ld", line, &row, &by))
        goto badline;
      code[last_code++] = (Ins){.type = ROTATE_ROW, .a = row, .b = by};
    } else if (str_startswith(line, str_constant("rotate column x="))) {
      long col, by;
      line = str_drop(line, 16);
      if (!str_scan("%ld by %ld", line, &col, &by))
        goto badline;
      code[last_code++] = (Ins){.type = ROTATE_COL, .a = col, .b = by};
    }
  }
  return;
 badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
}

bool disp[50*6] = {0};

#define BIT(x) ((uint64_t)1 << x)

void dump();

void run() {
  for (int i = 0; i < last_code; i++) {
    Ins c = code[i];
    switch (c.type) {
    case RECT: {
      for (uint8_t y = 0; y < c.b; y++) {
        for (uint8_t x = 0; x < c.a; x++) {
          disp[y*50+x] = 1;
        }
      }
      break;
    }
    case ROTATE_ROW: {
      int y = c.a;
      int by = c.b;
      char new_row[50];
      for (int x = 0; x < 50; x++) {
        new_row[x] = disp[y*50+((x+(50-by))%50)];
      }
      for (int x = 0; x < 50; x++) {
        disp[y*50+x] = new_row[x];
      }
      break;
    }
    case ROTATE_COL: {
      // column needs loop to rotate
      int x = c.a;
      int by = c.b % 8;
      char new_col[6];
      for (int y = 0; y < 6; y++) {
        new_col[y] = disp[((y+(6-by))%6)*50+x];
      }
      for (int y = 0; y < 6; y++) {
        disp[y*50+x] = new_col[y];
      }
      break;
    }
    }
  }
}

void dump() {
  printf("\n--------------\n");
  for (int y = 0; y < 6; y++) {
    printf("|");
    for (int x = 0; x < 50; x++) {
      printf("%c", (disp[y*50+x] ? '#' : '.'));
    }
    printf("\n");
  }
}

long on_count() {
  long on = 0;
  for (int i = 0; i < 50 * 6; i++)
    on += disp[i] ? 1 : 0;
  return on;
}

void day8(Day *day) {
  parse(day->input);
  run();
  dump();
  PART1(on_count()); // 94 is not right

}
