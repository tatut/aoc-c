#include "../aoc.h"

#include "../vendor/md5-c/md5.h"
#include "../vendor/md5-c/md5.c"
#include <stdint.h>

MD5Context salt;

#define UP    0
#define DOWN  1
#define LEFT  2
#define RIGHT 3

const char *dir = "UDLR";

typedef struct node {
  MD5Context md5;
  char path[64];
  u8 x, y;
  u16 len;
} node;

#define PATH(n, i) (((n).path[(i) / 32] >> (62 - (((i) % 32) * 2))) & (u64)0b11)
#define SET_PATH(n, i, d)                                                      \
  (n).path[(i) / 32] |= ((u64)(d) << (62 - (((i) % 32) * 2)))

inline char hex(uint8_t d) {
  if (d < 10)
    return '0' + d;
  else
    return 'a' + (d-10);
}

#define NIBBLE(md5, x) (0xf & (md5[(x) / 2] >> ((x) % 2 ? 0 : 4)))

void dfs(node n, u8 *shortest, Answer *p1, u16 *longest) {
  if (n.x == 3 && n.y == 3) {
    if(n.len < *shortest) {
      *shortest = n.len;
      set_str_answer(p1, (str){.len=n.len, n.path});
    }
    if (n.len > *longest) {
      *longest = n.len;
    }
    return;
  }

  MD5Context doors = n.md5;
  md5Finalize(&doors);

  for (int i = 0; i < 4; i++) {
    node n1 = n;
    if(NIBBLE(doors.digest, i) <= 10) continue; // door closed
    if (i == UP && n.y > 0) {
      n1.y = n.y - 1;
    } else if (i == DOWN && n.y < 3) {
      n1.y = n.y + 1;
    } else if (i == LEFT && n.x > 0) {
      n1.x = n.x - 1;
    } else if (i == RIGHT && n.x < 3) {
      n1.x = n.x + 1;
    } else {
      continue;
    }

    md5Update(&n1.md5, (u8 *)&dir[i], 1);
    if(n1.len < 64) n1.path[n1.len] = dir[i]; // don't update path after part1
    n1.len += 1;
    dfs(n1, shortest, p1, longest);
  }
}

void day17(Day *day) {
  str in = str_trim(day->input);
  md5Init(&salt);
  md5Update(&salt, (uint8_t*)in.data, in.len);
  u8 shortest = 255;
  u16 longest = 0;
  dfs((node){.md5 = salt}, &shortest, &day->part1, &longest);
  PART2((long)longest);
}
