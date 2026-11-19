#include "../aoc.h"

/* [X]
 * [Z]     [Y]
 *  1   2   3
 *
 * parse stacks, until we see the "1 2 3..." line
 */

typedef struct stack {
  char items[64];
  int size; // how many actual items (items[0] is bottom)
} stack;

typedef struct instr {
  int move; // how many to move
  int from; // from stack (1-9)
  int to;   // to stack (1-9)
} instr;

stack stacks[10] = {0}; // stacks 1...9 (0 is ignored)
stack stacks_copy[10];

instr instructions[1024];
int last_instruction = 0;

uint8_t col_stack[] = {[1] = 1,  [5] = 2,  [9] = 3,  [13] = 4, [17] = 5,
                       [21] = 6, [25] = 7, [29] = 8, [33] = 9};

void stack_push_bottom(stack *s, char item) {
  for (int i = s->size; i; i--) {
    s->items[i] = s->items[i-1];
  }
  s->size++;
  s->items[0] = item;
}

void dump() {
  for (int i = 1; i < 10; i++) {
    printf("%d",i);
    for (int s = 0; s < stacks[i].size; s++) {
      printf(" [%c]", stacks[i].items[s]);
    }
    printf("\n");
  }
}
// put value on top
void stack_put(stack *s, char item) { s->items[s->size++] = item; }
// take top value
char stack_take(stack *s) {
  return s->items[--s->size];
}

void stack_move(int from, int to) {
  stack_put(&stacks[to], stack_take(&stacks[from]));
}

void stack_move_many(int from, int to, int count) {
  int idx = stacks[from].size - count;
  for (int i = 0; i < count; i++) {
    stack_put(&stacks[to], stacks[from].items[idx+i]);
  }
  stacks[from].size -= count;
}

void parse(str lines) {
  str line;
  while(str_each_line(&lines, &line)) {
    if (str_startswith(line, str_constant(" 1 ")))
      break;
    for (int i = 0; i < line.len; i++) {
      if (line.data[i] >= 'A' && line.data[i] <= 'Z') {
        int idx = col_stack[i];
        stack_push_bottom(&stacks[idx], line.data[i]);
      }
    }
  }
  str_each_line(&lines, &line);
  if (line.len != 0) {
    fprintf(stderr, "Expected empty line after stacks, got: %.*s",
            (int)line.len, line.data);
    exit(1);
  }
  while (str_each_line(&lines, &line)) {
    long move, from, to;
    if (!str_scan("move %ld from %ld to %ld", line, &move, &from, &to))
      goto fail;
    instructions[last_instruction++] = (instr){.move = move, .from = from, .to = to};
  }
  return;
fail:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
  exit(1);
}

char top[9];

str top_row() {
  for (int i = 0; i < 9; i++) {
    top[i] = stacks[i+1].items[stacks[i+1].size-1];
  }
  return (str){.len=9,.data=top};
}

void day5(Day *day) {
  parse(day->input);
  memcpy(stacks_copy, stacks, sizeof(stack) * 10);

  for (int i = 0; i < last_instruction; i++) {
    for (int t = 0; t < instructions[i].move; t++) {
      stack_move(instructions[i].from, instructions[i].to);
    }
  }
  set_str_answer(&day->part1, top_row());

  // part2, move many
  memcpy(stacks, stacks_copy, sizeof(stack) * 10);
  for (int i = 0; i < last_instruction; i++) {
    stack_move_many(instructions[i].from, instructions[i].to, instructions[i].move);
  }
  set_str_answer(&day->part2, top_row());


}
