#include "../aoc.h"

// boo = bot or output, outputs are just after the bots, output id = 220+id
typedef struct boo {
  int values[2];
  int num_values;
  int low_to, high_to; // number to send low/high to (0 means no value)
} boo;

boo boos[250] = {0};
int max_bot = 0, max_out = 0;

void give_to(boo *b, int v) {
  if(b->num_values == 1 && b->values[0] == v) return;
  if(b->num_values == 2) {
    //fprintf(stderr, "Bot already has two values! %d and %d\n", b->values[0], b->values[1]);
    return;
  }
  b->values[b->num_values++] = v;
  if(b->num_values == 2 && b->values[0] > b->values[1]) {
    int tmp = b->values[1];
    b->values[1] = b->values[0];
    b->values[0] = tmp;
  }
}

void parse(str lines) {
  str line;
  while(str_each_line(&lines, &line)) {
    if(str_startswith(line, str_constant("bot"))) {
      long bot, low_to, high_to;
      str low_type, high_type;
      if(!str_scan("bot %ld gives low to %s %ld and high to %s %ld", line,
                   &bot, &low_type, &low_to, &high_type, &high_to)) goto badline;
      if(bot > max_bot) max_bot = bot;
      if(str_eq(low_type, str_constant("output"))) low_to += 220;
      if(str_eq(high_type, str_constant("output"))) high_to += 220;
      boos[bot].low_to = low_to;
      boos[bot].high_to = high_to;
    } else if(str_startswith(line, str_constant("value"))) {
      long bot, val;
      if(!str_scan("value %ld goes to bot %ld", line, &val, &bot)) goto badline;
      give_to(&boos[bot], val);
    }
  }

  return;
 badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
}


void trace(Day *day) {
  bool p1 = false, p2 = false;
  int p1_val, p2_val;
  while(!p1 || !p2) {
    for (int b = 0; b <= max_bot; b++) {
      boo *bot = &boos[b];
      if (bot->num_values == 2) {
        if (!p1 && bot->values[0] == 17 && bot->values[1] == 61) {
          p1 = true;
          PART1(b);
        }
        give_to(&boos[bot->low_to], bot->values[0]);
        give_to(&boos[bot->high_to], bot->values[1]);
      }
    }

    if (!p2 && boos[220].num_values && boos[221].num_values &&
        boos[222].num_values) {

      p2 = true;
      int p2v = boos[220].values[0] * boos[221].values[0] * boos[222].values[0];
      PART2(p2v);
    }
  }
}

void day10(Day *day) {
  parse(day->input);
  trace(day);
}
