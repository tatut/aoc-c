#include "../aoc.h"

// calculate A* distance between any two numbers
// then take shortest permutation of trips
#define W 179
#define H 43

char grid[W*H];

typedef struct pos {
  int x, y;
} pos;

typedef struct route {
  int to;
  int dist;
} route;

pos locs[8]; // 0=start, 1 through 7 are places to visit
route lens[8 * 8]; // from each starting pos to every other node

#define IDX(x,y) (((y)*W)+(x))
#define AT(x,y) (grid[IDX(x,y)])

void parse(str lines) {
  str line;
  int y = 0;
  while (str_each_line(&lines, &line)) {
    int x = 0;
    while (line.len) {
      char ch = line.data[0];
      line = str_drop(line,1);
      if (ch == '#' || ch == '.') {
        AT(x,y) = ch;
      } else if (ch >= '0' && ch <= '7') {
        AT(x,y) = '.';
        locs[ch-'0'] = (pos){.x = x, .y = y};
      } else {
        printf("Unrecognized data: %c (at x: %d, y: %d)\n", ch, x, y);
        exit(1);
      }
      x++;
    }
    y++;
  }
}

typedef struct node {
  pos p;
  int priority;
} node;

CMP_FN(node_cmp, node, priority);
EQ_FN(node_eq, node, a->p.x == b->p.x && a->p.y == b->p.y);

long distance(pos from, pos to) {
  return labs(from.x-to.x) + labs(from.y+to.y);
}

void print_visited(bool *visited) {
  for (int y = 0; y < H; y++) {
    for (int x = 0; x < W; x++) {
      if (visited[IDX(x, y)])
        printf("@");
      else printf("%c", AT(x,y));
    }
    printf("\n");
  }
}

void debug_pq(pqueue *pq) {
  printf("<|");
   // print queue items
    for(int i=0;i<pq->size;i++) {
      node *debug = (node*)PQ_PTR(pq,i);
      printf(" (%d,%d)pr:%d |", debug->p.x, debug->p.y, debug->priority);
    }
    printf(">\n");
}

long astar(int start_idx, int end_idx) {
  pos start = locs[start_idx];
  pos end = locs[end_idx];

  printf("A* from (%d,%d) TO (%d,%d)\n", start.x, start.y, end.x, end.y);
  pqueue open_set = pq_init(node, node_cmp, node_eq);
  long *fScore = malloc(sizeof(long) * W * H);
  long *gScore = malloc(sizeof(long) * W * H);
  bool *visited = calloc(W * H, sizeof(bool)); // use bitset
  for (int i = 0; i < W * H; i++) {
    fScore[i] = 99999999999;
    gScore[i] = 99999999999;
  }
  gScore[IDX(start.x, start.y)] = 0;
  fScore[IDX(start.x, start.y)] = distance(start, end);

  node n = (node){.p.x = start.x, .p.y = start.y, .priority = 0};
  pq_enq(open_set, n);
  long min_score = 999999999;
  while (open_set.size) {
    node at;
    pq_deq(open_set, at);
    visited[IDX(at.p.x, at.p.y)] = 1;
    //print_visited(visited);
    //printf("size: %zu at: (%d,%d), gScore: %ld, open: ", open_set.size, at.p.x,
    //       at.p.y, gScore[IDX(at.p.x, at.p.y)]);
    //debug_pq(&open_set);
    //printf("\n");

    if (at.p.x == end.x && at.p.y == end.y) {
      long score = gScore[IDX(at.p.x,at.p.y)];
      printf("found ! gScore: %ld\n", score);
      if(score < min_score) min_score = score;
      continue;
    }
    int d;
    for (int y = at.p.y - 1; y <= at.p.y + 1; y++) {
      for (int x = at.p.x - 1; x <= at.p.x + 1; x++) {
        d = (y == at.p.y ? 1 : 0) + (x == at.p.x ? 1 : 0);
        if(d != 1) continue;
        //printf(" char at candidate (%d,%d) = %c", x, y, AT(x,y));
        if (AT(x, y) != '#') {
          // passable corridor
          node to = (node){.p.x = x, .p.y = y};
          size_t idx = IDX(to.p.x,to.p.y);
          long tentative_gScore = gScore[IDX(at.p.x, at.p.y)] + 1;
          if (tentative_gScore < gScore[idx]) {
            //printf(" => better, enq (%ld < %ld)\n", tentative_gScore, gScore[idx]);
            // found better path
            // cameFrom, do we need it?
            gScore[idx] = tentative_gScore;
            fScore[idx] = tentative_gScore + distance(to.p, end);
            to.priority = fScore[idx];
            // if !contains
            if(!pq_contains(open_set, to))
              pq_enq(open_set, to);
            //printf("  after enq: "); debug_pq(&open_set); printf("\n");
          } //else printf("=> not better, skip (%ld >= %ld)\n", tentative_gScore, gScore[idx]);
        } //else printf(" => wall\n");
      }
    }
  }

  pq_free(&open_set);
  free(fScore);
  free(gScore);
  free(visited);
  return min_score;
}

long shortest[8 * 8];
#define SIDX(i, j) ((j) * 8 + (i))
#define SHORTEST(i, j) (shortest[((i) < (j) ? SIDX((i), (j)) : SIDX((j), (i)))])

void salesman(u8 visited, int at, long traveled, long *min_steps, long *min_steps_return) {
  if (visited == 0b11111111) {
    if (traveled < *min_steps)
      *min_steps = traveled;
    long with_return = traveled + SHORTEST(0, at);
    if (with_return < *min_steps_return)
      *min_steps_return = with_return;
  } else {
    // visit each unvisited place and recurse
    for (int i = 1; i <= 7; i++) {
      if(i == at) continue;
      u8 f = (u8)1 << i;
      if (!(visited & f)) {
        salesman(visited|f, i, traveled + SHORTEST(at,i), min_steps, min_steps_return);
      }
    }
  }
}

void day24(Day *day) {
  parse(day->input);

  for (int i = 0; i <= 7; i++) {
    for (int j = i+1; j <= 7; j++) {
      if (i == j)
        continue;
      long d = astar(i, j);
      printf("from location %d to %d, steps: %ld\n", i, j, d);
      SHORTEST(i,j) = d;
    }
  }

  long min_steps = 999999999999;
  long min_steps_return = 999999999999;
  salesman(1, 0, 0, &min_steps, &min_steps_return);
  PART1(min_steps);
  PART2(min_steps_return);

}
