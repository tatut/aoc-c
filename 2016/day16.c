#include "../aoc.h"

#define SIZE 35651584
#define SIZE_BYTES SIZE / 8


// reserve enough for part2 size
uint8_t disk[SIZE_BYTES] = {0}; // 34*8 == 272
uint8_t check[SIZE_BYTES] = {0};
size_t size = 0; // set this to desired size

#define GET(at,p) (at[(p) / 8] & ((uint8_t)1 << ((7 - ((p) % 8)))))
#define SET(at,p) (at[(p) / 8] |= ((uint8_t)1 << ((7 - ((p) % 8)))))


int dragon(int len) {
  int start = len + 1;
  int new_len = 2*len + 1;
  int p=len-1;
  for (int i = start; i < new_len && i < SIZE; i++) {
    if (!GET(disk,p)) {
      SET(disk,i);
    }
    p--;
  }
  return new_len;
}

int checksum(int len) {
  uint8_t in[SIZE_BYTES];
  memcpy(in, disk, SIZE_BYTES);

 start:
  bzero(check, SIZE_BYTES);
  int c = 0;
  for (int i = 0; i < len; i += 2) {
    if ((GET(in, i) && GET(in, i + 1)) || (!GET(in, i) && !GET(in, i + 1))) {
      SET(check, c);
    }
    c++;
  }
  if (c % 2 == 0) {
    memcpy(in, check, SIZE_BYTES);
    len = c;
    goto start;
  }
  return c;
}

void dump(uint8_t *data, int len) {
  printf("[len=%d :: ", len);
 for (int i = 0; i < len; i++) {
   printf("%c",  GET(data,i) ? '1' : '0');
 }
 printf(" ]\n");
}

int parse(str in) {
  int i = 0;
  in = str_trim(in);
  while (in.len) {
    if (in.data[0] == '1')
      SET(disk, i);
    i++;
    in = str_drop(in, 1);
  }
  return i;
}


void solve(Answer *a, str input, int sz) {
  bzero(disk, SIZE_BYTES);
  size = sz;
  int len = parse(input);
  while (len < size) {
    len = dragon(len);
  }
  int clen = checksum(size);

  char answer[64];
  int i = 0;
  while (i < clen) {
    answer[i] = GET(check, i) ? '1' : '0';
    i++;
  }
  set_str_answer(a, (str){.len = i, .data=answer});
}

void day16(Day *day) {
  solve(&day->part1, day->input, 272);
  solve(&day->part2, day->input, 35651584);
}
