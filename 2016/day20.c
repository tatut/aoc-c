#include "../aoc.h"

#define MAX_IP 4294967295l

typedef struct Range {
  long low, high;
} Range;

Range ranges[2000];
int last_range = 0;

CMP_FN(compare_range, Range, low)

void parse(str lines) {
  str line;
  while (str_each_line(&lines, &line)) {
    int i = last_range++;
    if(!str_scan("%ld-%ld", line, &ranges[i].low, &ranges[i].high)) goto badline;
  }
  qsort(ranges, last_range, sizeof(Range), compare_range);
  return;
badline:
  printf("Bad line: %.*s\n", (int)line.len, line.data);
}

long part1() {
  long ip = 0;
  int r = 0;
  bool blocked;
  do {
    blocked = false;
    for(int r=0; r<last_range; r++) {
      if (ranges[r].low <= ip && ranges[r].high >= ip) {
        if (ranges[r].high >= ip) {
          ip = ranges[r].high + 1;
        }
        blocked = true;
      }
    }
  } while(blocked);

  return ip;
}

long part2() {
  long allowed = 0;
  long ip = 0;
  int i = 0;
  while (ip < MAX_IP) {
    if (ip >= ranges[i].low) {
      if (ip <= ranges[i].high) {
        ip = ranges[i].high + 1;
      }
      i++;
    } else {
      allowed += 1;
      ip += 1;
    }
  }
  return allowed;
}

void day20(Day *day) {
  parse(day->input);
  PART1(part1());
  PART2(part2());
}
