#include "../aoc.h"

#define MAX_MONKEYS 10
#define MAX_ITEMS 32

typedef struct Monkey {
  long items[MAX_ITEMS];
  int item_first, item_last; // ring buffer
  int num_items;

  // Operation: new = old <op> <arg>
  char op;
  long arg;

  // Test: divisible by <div_by>
  long div_by;

  // throw to monkey index
  int if_true;
  int if_false;

  // total inspects count
  long inspected_count;
} Monkey;

Monkey monkeys[MAX_MONKEYS];
int last_monkey;

void add_item(Monkey *m, long item) {
  if(m->num_items == MAX_ITEMS) {
    fprintf(stderr, "monkey already has %d items, increase MAX_ITEMS\n", MAX_ITEMS);
    exit(1);
  }
  m->items[m->item_last] = item;
  m->item_last = (m->item_last + 1) % MAX_ITEMS;
  m->num_items++;
}

long take_item(Monkey *m) {
  if(m->num_items == 0) {
    fprintf(stderr, "Bug, you tried to take item from empty monkey\n");
    exit(1);
  }
  long item = m->items[m->item_first];
  m->item_first = (m->item_first + 1) % MAX_ITEMS;
  m->num_items--;
  return item;
}

void dump() {
  for (int i = 0; i < last_monkey; i++) {
    printf("| Monkey %d: ", i);
    int idx = monkeys[i].item_first;
    for (int j = 0; j < monkeys[i].num_items; j++) {
      printf("%c%ld", j ? ',' : ' ', monkeys[i].items[idx]);
      idx = (idx + 1) % MAX_ITEMS;
    }
    printf("\n");
  }
}

void dump_ins() {
  for (int i = 0; i < last_monkey; i++) {
    printf("Monkey %d inspected %ld times.\n", i, monkeys[i].inspected_count);
  }

}

void parse(str lines) {
  str line;
  last_monkey = 0;
  while (lines.len) {
    long idx, div_by, if_true, if_false;
    char op;
    str items,item,arg,rest;
    if (!str_scan("Monkey %ld:\n"
                  "  Starting items: %s\n"
                  "  Operation: new = old %c %s\n"
                  "  Test: divisible by %ld\n"
                  "    If true: throw to monkey %ld\n"
                  "    If false: throw to monkey %ld%*",
                  lines, &idx, &items, &op, &arg, &div_by, &if_true, &if_false, &rest))
      goto parse_fail;
    lines = str_ltrim(rest); // drop 2nd newline, if any
    Monkey m =
      (Monkey){.op=op, .div_by = div_by, .if_true = if_true, .if_false = if_false};
    if (str_eq(arg, str_constant("old")))
      m.arg = -1;
    else
      m.arg = str_to_long(arg);
    while (str_splitat_all(items, ",", &item, &items)) {
      int it = str_to_long(str_trim(item));
      //printf(" item: %d\n", it);
      add_item(&m, it);
    }
    monkeys[last_monkey++] = m;
  }

  return;
parse_fail:
  fprintf(stderr, "bad input starting at: %.*s\n", (int)lines.len, line.data);
  exit(1);
}

void run(long (relief)(long)) {
  for (int i = 0; i < last_monkey; i++) {
    //printf("Monkey %d inspecting %d items\n", i, monkeys[i].num_items);
    monkeys[i].inspected_count += monkeys[i].num_items;
    while (monkeys[i].num_items) {
      long item = take_item(&monkeys[i]);
      long arg = monkeys[i].arg;
      if(arg == -1) arg = item;
      switch (monkeys[i].op) {
      case '+':
        item += arg;
        break;
      case '*':
        item *= arg;
        break;
      default:
        fprintf(stderr, "Unknown op '%c'\n", monkeys[i].op);
        exit(1);
      }
      item = relief(item);
      if (item % monkeys[i].div_by == 0) {
        //printf("div by %d =>  item %d throw to %d\n", monkeys[i].div_by, item, monkeys[i].if_true);
        add_item(&monkeys[monkeys[i].if_true], item);
      } else {
        //printf("not div by %d =>  item %d throw to %d\n", monkeys[i].div_by, item, monkeys[i].if_false);
        add_item(&monkeys[monkeys[i].if_false], item);
      }
    }
  }
}

long solve(str input, int rounds, long (relief)(long)) {
  parse(input);
  for (int i = 1; i <= rounds; i++) {
    run(relief);
  }

  long top1=-1, top2=-1;
  for (int i = 0; i < last_monkey; i++) {
    long ins = monkeys[i].inspected_count;
    if (ins > top1) {
      top2 = top1;
      top1 = ins;
    } else if (ins > top2) {
      top2 = ins;
    }
  }
  return (top1 * top2);
}

long div3(long in) { return in / 3; }

long combined;
long rem_combined(long in) { return in % combined; }

void day11(Day *day) {
  PART1(solve(day->input, 20, &div3));

  combined = monkeys[0].div_by;
  for (int i = 1; i < last_monkey; i++)
    combined *= monkeys[i].div_by;

  PART2(solve(day->input, 10000, rem_combined));

}
