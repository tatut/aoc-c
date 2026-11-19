#include "../aoc.h"

bool overlaps_fully(long lo1, long hi1, long lo2, long hi2) {
  return (lo1 >= lo2 && hi1 <= hi2) ||
    (lo2 >= lo1 && hi2 <= hi1);
}

bool overlaps_some(long lo1, long hi1, long lo2, long hi2) {
  return (lo1 >= lo2 && lo1 <= hi2) || (hi1 >= lo2 && hi1 <= hi2) ||
    (lo2 >= lo1 && lo2 <= hi1) ||
    (hi2 >= lo1 && hi2 <= hi1);
}

void day4(Day *day) {
  str lines = day->input, line;
  int num_overlaps_fully = 0, num_overlaps_some = 0;

  while (str_each_line(&lines, &line)) {
    long lo1, hi1, lo2, hi2;
    if (!str_scan("%ld-%ld,%ld-%ld", line, &lo1, &hi1, &lo2, &hi2))
      goto fail;
    num_overlaps_fully += overlaps_fully(lo1, hi1, lo2, hi2);
    num_overlaps_some += overlaps_some(lo1,hi1,lo2,hi2);
  }

  PART1(num_overlaps_fully);
  PART2(num_overlaps_some);
  return;
fail:
  fprintf(stderr, "Invalid input: %.*s\n", (int)line.len, line.data);
  exit(1);
}
