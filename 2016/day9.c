#include "../aoc.h"

long len1(str in) {
  in = str_trim(in);
  long len = 0;
  while(in.len) {
    if(in.data[0] == '(') {
      str num, pos = str_drop(in, 1);
      str_splitat(pos, "x", &num, &pos);
      long chs = str_to_long(num);
      str_splitat(pos, ")", &num, &pos);
      long times = str_to_long(num);
      len += chs * times;
      in = str_drop(pos, chs);
    } else {
      len++;
      in = str_drop(in, 1);
    }
  }
  return len;
 badline:
  fprintf(stderr, "Bad line!\n");
  return -1;
}

long len2(str in) {
  in = str_trim(in);
  long len = 0;
  while(in.len) {
    if(in.data[0] == '(') {
      str num, pos = str_drop(in, 1);
      str_splitat(pos, "x", &num, &pos);
      long chs = str_to_long(num);
      str_splitat(pos, ")", &num, &pos);
      long times = str_to_long(num);
      len += len2(str_take(pos, chs))*times;
      in = str_drop(pos, chs);
    } else {
      len++;
      in = str_drop(in, 1);
    }
  }
  return len;
 badline:
  fprintf(stderr, "Bad line!\n");
  return -1;
}


void day9(Day *day) {
  PART1(len1(day->input));
  PART2(len2(day->input));
}
