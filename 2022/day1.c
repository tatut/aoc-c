#include "../aoc.h"


void day1(Day *day) {
  str lines, line;
  lines = day->input;

  int max1=0,max2=0,max3 = 0;
  int cals = 0;
  while (str_each_line(&lines, &line)) {
    if (line.len == 0) {
      if (cals > max3) {
        max3 = cals;
        if (max3 > max2) {
          cals = max3;
          max3 = max2;
          max2 = cals;
        }
        if (max2 > max1) {
          cals = max2;
          max2 = max1;
          max1 = cals;
        }
      }
      cals = 0;
    } else {
      cals += str_to_long(line);
    }
  }
  PART1(max1);
  PART2(max1+max2+max3);
}
