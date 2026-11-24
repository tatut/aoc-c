#include "../aoc.h"
#include <ctype.h>

typedef enum PType { LIST, INTEGER } PType;


typedef struct P {
  PType type;
  union {
    int value;
    struct P *head;
  };
  struct P *tail;
} P;

P *list_new() {
  P *n = NEW(P);
  n->type = LIST;
  n->head = NULL;
  return n;
}

#define TRIM *line = str_trim(*line)
#define CONSUME *line = str_drop(*line,1);

P *read_list(str *line) {
  TRIM;
  if (str_char_at(*line, 0) != '[')
    return NULL;
  CONSUME;
  P *head = NEW0(P);
  head->type = LIST;
  head->head = NULL;

  P *prev = NULL, *n;
  while (str_char_at(*line, 0) != ']') {
    // either a list or integer
    char at = str_char_at(*line, 0);
    if (at == '[') {
      n = read_list(line);
    } else if (isdigit(at)) {
      n = NEW0(P);
      n->type = INTEGER;
      n->value = str_to_long_rest(*line, line);
      n->tail = NULL;
      //printf(" read int: <<%d>>, rest: '%.*s'\n", n->value, (int)line->len, line->data);
    } else {
      fprintf(stderr, "Unexpected: '%c'\n", str_char_at(*line, 0));
      exit(1);
    }
    if(!head->head) {
      head->head = n;
      prev = n;
    } else {
      prev->tail = n;
      prev = n;
    }
    // consume comma, if we have more items
    if(str_char_at(*line,0) == ',') CONSUME;
  }
  if (str_char_at(*line, 0) != ']') {
    fprintf(stderr, "Expected ']' to end list, got: '%c'\n",
            str_char_at(*line, 0));
    exit(1);
  }
  CONSUME;
  return head;
}

void dump(P *item) {
  if (item->type == INTEGER)
    printf("%d", item->value);
  else {
    printf("[");
    bool first = true;
    P *at = item->head;
    while (at) {
      if(!first) printf(",");
      dump(at);
      at = at->tail;
      first = false;
    }
    printf("]");
  }
}

typedef enum Order { YES, NO, UNK } Order;  // UNK means unknown now

Order compare(P *l, P *r) {
#define OUT(x)                                                                 \
  do {                                                                         \
    ret = (x);                                                                 \
    goto done;                                                                 \
  } while (false)
  Order ret;
  P *l_orig = l, *r_orig = r;
  if (l->type == INTEGER && r->type == INTEGER) {
    if(l->value < r->value)
      OUT(YES);
    else if (l->value > r->value)
      OUT(NO);
    else
      OUT(UNK);
  } else {
    // convert non list to list
    P tmp1,tmp2;
    if (l->type == LIST)
      l = l->head;
    else {
      // number to list of single element list (clear the tail)
      tmp1 = *l;
      tmp1.tail = NULL;
      l = &tmp1;
    }

    if (r->type == LIST)
      r = r->head;
    else {
      tmp2 = *r;
      tmp2.tail = NULL;
      r = &tmp2;
    }

    while (l && r) {
      Order o = compare(l, r);
      if (o != UNK)
        OUT(o);
      l = l->tail;
      r = r->tail;
    }
    if (!l && r)
      OUT(YES); // left ran out
    if (l && !r)
      OUT(NO); // right ran out
    OUT(UNK); // both ran out
  }
  fprintf(stderr, "undecidable?\n");
  exit(1);
done:
  // dump(l_orig); printf(" vs "); dump(r_orig);
  // printf(" => %s\n", ret == YES ? "YES" : (ret == NO ? "NO" : "UNK"));
  return ret;
}

P *read_packet(str line) {
  P *root = read_list(&line);
  //printf("got root: %p, lin len: %zu\n", root, line.len);
  if(!root || line.len) goto fail;
  return root;
fail:
  fprintf(stderr, "Bad input: %.*s\n", (int)line.len, line.data);
  return NULL;
}

int qs_compare(const void *lptr, const void *rptr) {
  P *l = *(P **)lptr;
  P *r = *(P **)rptr;
  switch (compare(l, r)) {
  case YES:
    return -1;
  case NO:
    return 1;
  case UNK:
    return 0;
  }
}

void day13(Day *day) {
  // P *p = read_packet(str_constant("[1,2,3,[420,666],9]"));
  // dump(p);
  str lines = day->input, line1, line2;
  int idx = 1;
  int ordered_idx_sum = 0;
  P *all_packets[500];
  P *div1 = read_packet(str_constant("[[2]]"));
  P *div2 = read_packet(str_constant("[[6]]"));
  all_packets[0] = div1;
  all_packets[1] = div2;
  int last_packet = 2;

  while (lines.len) {
    if(!str_each_line(&lines, &line1)) goto fail;
    if(!str_each_line(&lines, &line2)) goto fail;
    P *p1 = read_packet(line1);
    P *p2 = read_packet(line2);
    all_packets[last_packet++] = p1;
    all_packets[last_packet++] = p2;

    Order c = compare(p1, p2);
    if (c == YES)
      ordered_idx_sum += idx;

    lines = str_trim(lines);
    idx++;
  }
  PART1(ordered_idx_sum);

  /* Sort all packets (including dividers) */
  qsort(all_packets, last_packet, sizeof(P *), qs_compare);
  int decoder = 1;
  for (int i = 0; i < last_packet; i++) {
    if (all_packets[i] == div1 || all_packets[i] == div2)
      decoder *= (i+1);
  }
  PART2(decoder);

  return;
fail:
  fprintf(stderr, "Bad data: %.*s\n", (int)lines.len, lines.data);
}
