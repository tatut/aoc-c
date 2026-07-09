#include "../aoc.h"

typedef enum OpType {
  SWAP_POS,
  SWAP_CHAR,
  ROTATE_BY,
  ROTATE_LEFT,
  ROTATE_RIGHT,
  REVERSE,
  MOVE
} OpType;

typedef struct Op {
  OpType type;
  u8 a, b;
} Op;

// data is an array of indexes for each character

Op ops[128];
int last_op = 0;

void parse(str lines) {
  str line;
  while (str_each_line(&lines, &line)) {
    int i = last_op++;
    char ach, bch;
    long a, b;
    switch (str_scan_any(line, 7, "swap position %ld with position %ld", (void *[]){&a, &b},
                         "swap letter %c with letter %c",  (void *[]){&ach, &bch},
                         "rotate based on position of letter %c", (void*[]){&ach},
                         "rotate left %ld steps", (void *[]){&a},
                         "rotate right %ld steps", (void *[]){&a},
                         "reverse positions %ld through %ld", (void *[]){&a, &b},
                         "move position %ld to position %ld", (void *[]){&a, &b})) {
    case SWAP_POS:
      ops[i] = (Op){.type = SWAP_POS, .a = a, .b = b};
      break;
    case SWAP_CHAR:
      ops[i] = (Op){.type = SWAP_CHAR, .a = ach, .b = bch};
      break;
    case ROTATE_BY:
      ops[i] = (Op){.type = ROTATE_BY, .a = ach};
      break;
    case ROTATE_LEFT:
      ops[i] = (Op){.type = ROTATE_LEFT, .a = a};
      break;
    case ROTATE_RIGHT:
      ops[i] = (Op){.type = ROTATE_RIGHT, .a = a};
      break;
    case REVERSE:
      ops[i] = (Op){.type = REVERSE, .a = a, .b = b};
      break;
    case MOVE:
      ops[i] = (Op){.type = MOVE, .a = a, .b = b};
      break;
    case -1:
      fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
      return;
    }
  }
}

void rotate(str s, int by) {
  char buf[8];
  if(by < 0) by = 8 + by;
  for (int i = 0; i < s.len; i++) {
    int p = (i + by) % 8;
    buf[p] = s.data[i];
  }
  memcpy(s.data, buf, s.len);
}

void scramble(str s, Op op) {
  char buf[12];
  switch (op.type) {
  case SWAP_CHAR: {
    int ap, bp;
    for (int i = 0; i < s.len; i++) {
      if (s.data[i] == op.a)
        ap = i;
      else if (s.data[i] == op.b)
        bp = i;
    }
    op.a = ap;
    op.b = bp;
    // fall through to swap pos
  }
  case SWAP_POS: {
    char tmp = s.data[op.a];
    s.data[op.a] = s.data[op.b];
    s.data[op.b] = tmp;
    break;
  }
  case ROTATE_BY: {
    char by = op.a;
    for (int i = 0; i < s.len; i++) {
      if (s.data[i] == by) {
        rotate(s, i + 1 + (i >= 4 ? 1 : 0));
        return;
      }
    }
    break;
  }
  case ROTATE_RIGHT: {
    rotate(s, op.a);
    break;
  }
  case ROTATE_LEFT: {
    rotate(s, -op.a);
    break;
  }
  case REVERSE: {
    int i = op.a;
    int j = op.b;
    while (i < j) {
      char tmp = s.data[i];
      s.data[i] = s.data[j];
      s.data[j] = tmp;
      i++; j--;
    }
    break;
  }
  case MOVE: {
    char m = s.data[op.a];
    int b = 0;
    for (int i = 0; i < s.len; i++) {
      if(i == op.a) continue;
      buf[b++] = s.data[i];
    }
    b = 0;
    for (int i = 0; i < s.len; i++) {
      if (i == op.b)
        s.data[i] = m;
      else
        s.data[i] = buf[b++];
    }
  }
  }
}

// get next permutation, 3 bits per idx
void permute(str data, str out, u32 *perm) {
  u32 p = *perm;
  u32 used;
 start:
   used = 0;
   u32 bits = p;
  for (int i = 0; i < 8; i++) {
    u32 idx = (bits & 0b111);
    u32 f = (u32)1 << idx;
    if (f & used) {
      p++;
      goto start;
    }
    used |= f;
    bits = bits >> 3;
    out.data[i] = data.data[idx];
  }
  *perm = p;
}

void debug_check(str s, int op) {
  bool debug[26] = {0};
  for (int i = 0; i < s.len; i++) {
    if (debug[s.data[i] - 'a']) {
      printf("%.*s has repeated char %c at %d after op %d\n", (int)s.len, s.data, s.data[i],
             i, op);
      exit(1);
    }
    debug[s.data[i]-'a'] = 1;
  }
}

void scramble_all(str s) {
 for(int i=0;i<last_op;i++) {
   scramble(s, ops[i]);
   debug_check(s, i);
 }
}
void day21(Day *day) {
  parse(day->input);

  char data[] = "abcdefgh";
  str s = (str){.data = data, .len = 8};
  scramble_all(s);
  set_str_answer(&day->part1, s);

  // try all options to find fbgdceah
  char scrambled_data[] = "fbgdceah";
  char permutation[8];
  char answer_data[8];
  str scramble = (str){.data = scrambled_data, .len = 8};
  str original = (str){.data = permutation, .len = 8};
  str answer = (str){.data = answer_data, .len = 8};
  u32 p = 0;
  while (true) {
    permute(scramble, original, &p);
    memcpy(answer.data, original.data, 8);

    scramble_all(original);
    if (str_eq(scramble, original)) {
      set_str_answer(&day->part2, answer);
      break;
    }
    p++;

  }

}
