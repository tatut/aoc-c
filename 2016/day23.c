#include "../aoc.h"

typedef enum Op { CPY, JNZ, INC, DEC, TGL } Instr;

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

typedef struct jnz_data  {
  u64 calls; // how many times we've been here
  long reg[4]; // register values the previous time
} jnz_data;



void run(long *reg) {
  jnz_data jnzd[32] = {0};
  int pc = 0;
  while (pc >= 0 && pc < last_line) {
    str line = lines[pc];
    char op = line.data[0];
    //printf("[%02d] %.*s (A: %ld, B: %ld, C: %ld, D: %ld) => \t", pc, (int)line.len, line.data, reg[0],reg[1],reg[2],reg[3]);

    str args = str_drop(lines[pc], 4);
    //    printf("ARGS: '%.*s'\n", (int)args.len, args.data);
    switch (op) {
    case 'c': { // cpy
      //printf("cpy");
      long v = reg_or_num(&args, reg);
      int r = args.data[0]-'a';
      if(r < 0 || r > 3)
        printf("invalid cpy instr!\n");
      else
        reg[r] = v;
      pc++;
      break;
    }
    case 'i': { // inc
      //printf("inc");
      int r = args.data[0]-'a';
      if(r < 0 || r > 3)
        printf("invalid inc instr!\n");
      else
        reg[r]++;
      pc++;
      break;
    }
    case 'd': { // dec
      //printf("dec");
      int r = args.data[0]-'a';
      if(r < 0 || r > 3)
        printf("invalid dec instr!\n");
      else
        reg[r]--;
      pc++;
      break;
    }
    case 'j': { // jnz
      /*str jargs = args;
      if (jargs.data[0] >= 'a' && jargs.data[0] <= 'd') {
        int r = jargs.data[0] - 'a';
        if (!r) {
          jnzd[pc] = (jnz_data){0};
        } else {
          // try to optimize
          jnzd[pc].calls++;
          if (jnzd[pc].calls > 1) {
            int diff = 0;
            int diff_reg = -1;
            long diff_by;
            for (int i = 0; i < 4; i++) {
              if (i != r && jnzd[pc].reg[i] != reg[i]) {
                //  printf("| %c: %ld -> %ld", i + 'a', jnzd[pc].reg[i], reg[i]);
                diff_reg = i;
                diff++;
                diff_by = reg[i] - jnzd[pc].reg[i];
              }
            }
            printf("d:%d    JNZ[pc=%d] cmp:%c,  regs: %ld, %ld, %ld, %ld\n", diff,  pc,
                   r + 'a', reg[0], reg[1], reg[2], reg[3]);
            if (diff == 1) {
              // one register differs, optimize
              reg[diff_reg] += diff_by * reg[r];
              reg[r] = 0;
              jnzd[pc] = (jnz_data){0};
            }
          }
          memcpy(jnzd[pc].reg, reg, sizeof(long) * 4);
        }
      }*/
      long v = reg_or_num(&args, reg);
      if (v) {
        long jmp = reg_or_num(&args, reg);
        pc += jmp;
      } else {
        pc++;
      }
      break;
    }
    case 't': {

      int r = args.data[0]-'a';
      if(r < 0 || r > 3)
        printf("invalid tgl instr!\n");
      else {
        int pos = pc + reg[r];
        printf("pc %d + reg[%c] %ld => %d\n", pc, r+'a', reg[r], pos);
        if(pos >= 0 && pos < last_line) {
          str l = lines[pos];
          switch(l.data[0]) {
          case 'c': l.data[0] = 'j'; break;
          case 'i': l.data[0] = 'd'; break;
          case 'd': l.data[0] = 'i'; break;
          case 'j': l.data[0] = 'c'; break;
          case 't': l.data[0] = 'i'; break;
          }
        }
      }
      pc++;
      break;
    }
    default:
      printf("unknown instruction: %c\n", op);
      exit(1);
    }
    //printf(" => Regs: %ld, %ld, %ld, %ld\n", reg[0],reg[1],reg[2],reg[3]);
  }
}

void parse(str in_lines) {
  str line;
  while(str_each_line(&in_lines, &line)) {
    lines[last_line++] = line;
  }
}

void day23(Day *day) {
  parse(str_dup(day->input));
  long reg1[4] = {7,0,0,0};
  run(reg1);
  PART1(reg1[0]);

  parse(str_dup(day->input));
  long reg2[4] = {12,0,0,0};
  run(reg2);
  PART2(reg2[0]);



}
