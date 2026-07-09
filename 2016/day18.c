#include "../aoc.h"

str lines[40];

int safes(str line) {
  int s=0;
  for (int i = 0; i < line.len; i++)
    s += line.data[i] == '.';
  return s;
}

void next_line(str line, str next) {
  char l, c, r;
  for (int i = 0; i < next.len; i++) {
    l = (i == 0 ? '.' : line.data[i - 1]);
    c = line.data[i];
    r = (i == next.len - 1 ? '.' : line.data[i + 1]);

    if ((l == '^' && c == '^' && r == '.') ||
        (l == '.' && c == '^' && r == '^') ||
        (l == '^' && c == '.' && r == '.') ||
        (l == '.' && c == '.' && r == '^')) {
      next.data[i] = '^';
    } else {
      next.data[i] = '.';
    }
  }
}

void day18(Day *day) {
  str line = str_trim(day->input);
  char data[128];
  str next = (str){.len = line.len, .data = data};
  str tmp;

  long safe = safes(line);
  int i = 1;
  while(1) {
    next_line(line, next);
    safe += safes(next);
    i++;
    if (i == 40) {
      PART1(safe);
    } else if (i == 400000) {
      PART2(safe);
      break;
    }
    tmp = line;
    line = next;
    next = tmp;
  }
}
