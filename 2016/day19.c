#include "../aoc.h"

typedef struct Elf {
  int num;
  int gifts;
  struct Elf *left, *right;
} Elf;

long part1(int elves) {
  long *elf = malloc(sizeof(long) * elves);
  for (int i = 0; i < elves; i++) {
    elf[i] = 1;
  }

  while (true) {
    for (int i = 0; i < elves; i++) {
      if(!elf[i]) continue;
      int steal = (i+1) % elves;
      while(!elf[steal]) steal = (steal+1) % elves;
      elf[i] += elf[steal];
      elf[steal] = 0;
      if (elf[i] == elves) {
        return i+1;
      }
    }
  }
}

Elf *kick(Elf *who, int playing) {
  Elf *r = who->right;
  Elf *l = who->left;
  r->left = l;
  l->right = r;

  who = l;
  if(playing % 2 == 1)
    who = who->left;

  return who;
}

long part2(int elves) {
  Elf *elf = malloc(elves * sizeof(Elf));
  Elf *steal = &elf[elves/2];
  for (int i = 0; i < elves; i++) {
    elf[i] = (Elf){.num = i + 1,
                   .gifts = 1,
                   .left = &elf[(i + 1) % elves],
                   .right = &elf[(i - 1) % elves]};
  }
  int playing = elves;
  Elf *e = &elf[0];
  while (playing) {
    e->gifts += steal->gifts;
    if (e->gifts == elves) {
      return e->num;
    }
    steal = kick(steal, playing);
    e = e->left;
    playing--;
  }
  return -1;
}

void day19(Day *day){
  int elves = (int) str_to_long(str_trim(day->input));
  PART1(part1(elves));
  PART2(part2(elves));
}
