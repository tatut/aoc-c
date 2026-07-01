#include "../aoc.h"

typedef uint64_t st;

str types[6] = {0};
int last_type = 0;

int type_of(str t) {
  for(int i=0;i<last_type;i++) {
    if(str_eq(t,types[i])) return i;
  }
  int idx = last_type++;
  types[idx] = t;
  return idx;
}

// state is a single uint64 as:
// high 4 bits are the elevator floor (0 - 4)
// 10 bits per floor, 1st floor is lower bits
// bits contain alternating chip and generator
// eg, if there's types: A B C D E and a floow as B microchip and D generator
// AMAGBMBGCMCGDMDGEMEG
//  0 0 1 0 0 0 0 1 0 0
//
// a configuration is "safe" if no generator has corresponding chip
// or every chip is powered by its own generator
//
// [20 empty][E(4)][floor1(10)][floor2(10)][floor3(10)][floor4(10)]
//  ........  0001  0000000000    0000000000 00000000000  0000000000

// A* through any "safe" neighbor, always just 1 floor up/down
// heuristic is how many items are on 4th floor

#define ELEVATOR(st) (st >> 40)
#define FLOOR_CONTENTS(st, f) ((st >> ((3 - f) * 10)) & 0b1111111111)
#define HAS_MICROCHIP(fc, t) (fc & (st)1 << (t * 2 + 1))
#define HAS_GENERATOR(fc, t) (fc & (st)1 << (t * 2 + 0))
#define FLOOR_ITEM_BIT(f, c) ((st)1 << ((3 - (f)) * 10 + (c)))

bool is_safe(st state) {
  for(int i=0;i<4;i++) {
    st floor = FLOOR_CONTENTS(state, i);
    int chips = 0, gens = 0;
    bool unpaired=false;
    for(int c=0;c<5;c++) {
      if(HAS_MICROCHIP(floor, c)) {
        chips++;
        if (!HAS_GENERATOR(floor, c)) {
          unpaired = true;
        }
      }
      if(HAS_GENERATOR(floor, c)) gens++;
    }
    if(gens && unpaired) return false;
  }
  return true;
}


long popcount(uint64_t n) {
  long count = 0;
  while (n) {
    n &= (n - 1);
    count++;
  }
  return count;
}

// check that no item is on 2 floors
void dump(st state);
void debug(st state) {
  int items = 0;
  for (int i = 0; i < 4; i++) {
    items += popcount(FLOOR_CONTENTS(state,i));
  }
  if (items != 10) {
    printf("BUG, item on more than 1 floor\n");
    dump(state);
    exit(1);
  }
}


bool is_done(st state) {
  long pc = popcount(FLOOR_CONTENTS(state, 3));
  //printf("is done? popcount %ld\n", pc);
  return pc == 10;
}

uint64_t parse(str lines) {
  st state = 0; // elevator on 1st floor (0)
  int floor = 0;
  str line;
  while (str_each_line(&lines, &line)) {
    uint64_t contents = 0; // low 10 bits, contents on this floor
    // find each " a " in the line
    while (line.len > 3) {
      if (str_startswith(line, str_constant(" a "))) {
        line = str_drop(line, 3);
        str type;
        if (!str_splitat(line, " -", &type, &line))
          goto badline;
        int t = type_of(type);
        if (str_startswith(line, str_constant("generator"))) {
          //printf("floor %d: %.*s GENERATOR\n", floor, (int)type.len, type.data);
          contents |= (uint64_t)1<<(t*2+0);
          line = str_drop(line, 9);
        } else if (str_startswith(line, str_constant("compatible microchip"))) {
          //printf("floor %d: %.*s MICROCHIP\n", floor, (int)type.len, type.data);
          contents |= (uint64_t)1<<(t*2+1);
          line = str_drop(line, 20);
        } else {
          goto badline;
        }
      }
      line = str_drop(line, 1);
    }
    floor++;
    state = (state << 10) | contents;
  }

  return state;
badline:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
  return 0;
}

void dump(st state) {
  uint64_t elevator = state >> 40;
  printf("------------\n");
  for (int f = 3; f >= 0; f--) {
    printf("[%d] %s", f+1,  elevator == f ? "E" : " ");
    uint64_t contents = state >> ((3 - f) * 10);
    for (int t = 0; t < 5; t++) {
      if (contents & ((uint64_t)1 << (t * 2 + 0)))
        printf(" %.*sG", 2, types[t].data);
      else
        printf("    ");
      if (contents & ((uint64_t)1 << (t * 2 + 1)))
        printf(" %.*sM", 2, types[t].data);
      else
        printf("    ");
    }
    printf("\n");
  }
}

void dump2(st s1, st s2) {
  uint64_t e1 = s1 >> 40;
  uint64_t e2 = s2 >> 40;
  printf("------------\n");
  for (int f = 3; f >= 0; f--) {
    printf("[%d] %s", f+1,  e1 == f ? "E" : " ");
    uint64_t contents = FLOOR_CONTENTS(s1, f);
    for (int t = 0; t < 5; t++) {
      if (contents & ((uint64_t)1 << (t * 2 + 0)))
        printf(" %.*sG", 2, types[t].data);
      else
        printf("    ");
      if (contents & ((uint64_t)1 << (t * 2 + 1)))
        printf(" %.*sM", 2, types[t].data);
      else
        printf("    ");
    }

    printf(" | [%d] %s", f+1,  e2 == f ? "E" : " ");
    contents = FLOOR_CONTENTS(s2, f);
    for (int t = 0; t < 5; t++) {
      if (contents & ((uint64_t)1 << (t * 2 + 0)))
        printf(" %.*sG", 2, types[t].data);
      else
        printf("    ");
      if (contents & ((uint64_t)1 << (t * 2 + 1)))
        printf(" %.*sM", 2, types[t].data);
      else
        printf("    ");
    }

    printf("\n");
  }
}



#include "../stb_ds.h"

typedef struct node {
  st from, to;
  int steps;
} node;

typedef struct Vis {
  st from, to;
  //  int steps;
} Vis;

typedef struct Visited {
  Vis key;
  bool value;
} Visited;

Vis key(st from, st to, int _steps) {
  return (Vis){.from = from, .to = to}; //, .steps = steps};
}

/* Fill possible safe neighbors to given array */
int bfs(st initial) {
  queue q = initq(node);

  Visited *visited = NULL;

  node n = (node){.from = 0, .to = initial, .steps = 0};
  enq(q, n);

  int min_steps = 999999;
  int max_pc = 0;

  while (q.size) {
    n = deq(q, node);

    bool show_children = false;
    // if (is_done(n.to)) {
    if(popcount(FLOOR_CONTENTS(n.to, 3)) == 10) {
      //printf("\ndone in %d steps\n", n.steps);
      //dump2(n.from, n.to);
      if (n.steps < min_steps) {
        min_steps = n.steps;
      }
      continue;
    }
    st current = n.to;
    // move up/down taking 1 or 2 items with you
    int e = ELEVATOR(current);
    st contents = FLOOR_CONTENTS(current, e);
    // take 1 or 2 items to new floor
    int children = 0;
    for (int i = 0; i < 10; i++) {
      if (contents & ((st)1 << i)) {
        // take this item up/down
        st next = current & ~(FLOOR_ITEM_BIT(e, i)); // remove from old floor
        next = (next << 24) >> 24; // clear elevator
        for (int f = e - 1; f <= e + 1; f++) {
          if (f < 0 || f > 3 || f == e)
            continue; // impossible floor
          st next1 = next;
          next1 = next1 | FLOOR_ITEM_BIT(f, i); // add item
          next1 = next1 + ((st)f << 40); // set elevator
          if (is_safe(next1)) {
            Vis k = key(current, next1, n.steps+1);
            if (hmgeti(visited, k) == -1) {
              hmput(visited, k, true);
              // printf(" go from floor %d to %d with 1 item\n", e, f);
              // dump2(current, next1);
              node n1 = (node){.from = n.to, .to=next1, .steps=n.steps+1};
              enq(q, n1);
              children++;
            }
          }
          // take another item as well
          for (int j = i + 1; j < 10; j++) {
            st next2 = next1;
            if (contents & ((st)1 << j)) {
              next2 = next2 & ~(FLOOR_ITEM_BIT(e, j)); // remove from old floor
              next2 = next2 | FLOOR_ITEM_BIT(f, j); // add item
              if (is_safe(next2)) {
                Vis k = key(current, next2, n.steps+1);
                if (hmgeti(visited, k) == -1) {
                  hmput(visited, k, true);
                  // printf(" go from floor %d to %d with 2 items\n", e, f);
                  // dump2(current,next2);
                  node n2 = (node){.from = n.to, .to=next2, .steps=n.steps+1};
                  enq(q, n2);
                  children++;
                }
              }
            }
          }
        }
      }
    }
  }
  return min_steps;
}

void day11(Day *day) {
  st state = parse(day->input);
  //dump(state);
  int min_steps = bfs(state);
  PART1(min_steps); // 33 is correct
}
