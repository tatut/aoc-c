#include "../aoc.h"

int priority(char item) {
  if(item >= 'a' && item <= 'z') {
    return 1+(item-'a'); // a=1 ... z=26
  } else if (item >= 'A' && item <= 'Z') {
    return 27+(item-'A'); // A=27 ... Z=52
  } else {
    fprintf(stderr, "Unrecognized item '%c'.\n", item);
    exit(1);
  }
}

void day3(Day *day) {
  str lines = day->input, line;
  int priority_sum=0;
  while (str_each_line(&lines, &line)) {
    int half = line.len/2;
    for (int i = 0; i < half; i++) {
      for (int j = half; j < line.len; j++) {
        if (line.data[i] == line.data[j]) {
          priority_sum += priority(line.data[i]);
          goto next;
        }
      }
    }
  next:;
  }

  PART1(priority_sum);

  lines = day->input;
  int badge_sum = 0;
  while (lines.len) {
    str elves[3];
    if(!str_each_line(&lines, &elves[0])) goto fail;
    if(!str_each_line(&lines, &elves[1])) goto fail;
    if(!str_each_line(&lines, &elves[2])) goto fail;
    uint8_t occurs[53];
    bzero(occurs, 53);
    for (int e = 0; e < 3; e++) {
      for (int i = 0; i < elves[e].len; i++) {
        int p = priority(elves[e].data[i]);
        occurs[p] |= (1 << e);
        if (occurs[p] == 7) {
          badge_sum += p;
          goto next2;
        }
      }
    }
  next2:;
  }

  PART2(badge_sum);
  return;
fail:
  fprintf(stderr, "Expected line in elf group\n");
  exit(1);
}
