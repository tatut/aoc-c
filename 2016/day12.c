#include "../aoc.h"

typedef enum Op { CPY, JNZ, INC, DEC } Instr;

str lines[32];

int last_line = 0;

inline long reg_or_num(str *args_ptr, long *reg) {
  long v = 0;
  str args = *args_ptr;
  if (args.data[0] >= 'a' && args.data[0] <= 'd') {
    // register name
    v = reg[args.data[0] - 'a'];
    *args_ptr = str_drop(args, 2);
  } else {
    v = str_to_long_rest(args, &args);
    *args_ptr = str_drop(args, 1);
  }
  return v;
}

void run(long *reg) {
  int pc = 0;
  while (pc >= 0 && pc < last_line) {
    char op = lines[pc].data[0];
    str args = str_drop(lines[pc], 4);
    //    printf("ARGS: '%.*s'\n", (int)args.len, args.data);
    switch (lines[pc].data[0]) {
    case 'c': { // cpy
      long v = reg_or_num(&args, reg);
      reg[args.data[0]-'a'] = v;
      pc++;
      break;
    }
    case 'i': { // inc
      reg[args.data[0]-'a']++;
      pc++;
      break;
    }
    case 'd': { // dec
      reg[args.data[0]-'a']--;
      pc++;
      break;
    }
    case 'j': { // jnz
      long v = reg_or_num(&args, reg);
      if (v) {
        long jmp = str_to_long(args);
        pc += jmp;
      } else {
        pc++;
      }
      break;
    }
    default:
      printf("unknown instruction: %c\n", op);
      exit(1);
    }
  }
}



void day12(Day *day){
  str line, in_lines = day->input;
  while (str_each_line(&in_lines, &line)) {
    lines[last_line++] = line;
  }

  long reg1[4] = {0};
  run(reg1);
  PART1(reg1[0]);

  long reg2[4] = {0,0,1,0};
  run(reg2);
  PART2(reg2[0]);

}
