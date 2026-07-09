#include "aoc.h"
#include <stdarg.h>

void set_long_answer(Answer *a, long ans) {
  a->type = LONG_ANSWER;
  a->answer.long_answer = ans;
}

void set_str_answer(Answer *a, str ans) {
  a->type = STR_ANSWER;
  a->answer.str_answer = str_dup(ans);
}

#define STB_DS_IMPLEMENTATION
#include "stb_ds.h"

static bool _scan_report_error = true;
static bool _scan_array = false; // if args are in a void* array

#define ARG(type)                                                              \
  (_scan_array ? ((type)_scan_args[_scan_arg_idx++]) : (va_arg(args, type)))


bool str_scan(const char *fmt, str in, ...) {
  va_list args;
  va_start(args, in);
  void **_scan_args = {0};
  int _scan_arg_idx = 0;
  if (_scan_array) {
    _scan_args = va_arg(args, void**);
  }
  const char *at = fmt;
  while (in.len) {
    //printf("SCAN:\n  >>%s\n  >>%.*s\n", at, (int)in.len, in.data);
    if(*at != '%') {
      if(*at != in.data[0]) goto fail;
      in = str_drop(in, 1);
      at++;
    } else {
      // a format specifier like %s
      at++;
      if(*at == 's') {
        str *to = va_arg(args, str*);
        // read input until the next char (or end of string)
        char delim[2] = {0,0};
        if (*(at + 1) == 0) {
          // end of input, the string is the whole rest
          *to = in;
          goto done;
        } else {
          delim[0] = *(at+1);
        }
        if(!str_splitat(in, delim, to, &in)) goto fail;
        at += 2;
      } else if (*at == 'l' && *(at + 1) == 'd') {
        at += 2;
        // read long
        if (in.data[0] == '+' || in.data[0] == '-' || is_digit(in.data[0])) {
          long *to = ARG(long*);
          *to = str_to_long_rest(in, &in);
        } else {
          goto fail;
        }
      } else if (*at == '%') {
        // literal %
        if (in.data[0] != '%')
          goto fail;
        in = str_drop(in, 1);
      } else if (*at == 'c') {
        // literal char
        char *to = ARG(char *);
        *to = in.data[0];
        at++;
        in = str_drop(in, 1);
      } else if (*at == 'w') {
        // skip any amount of whitespace (space, tab, newline)
        at++;
        char ch = in.data[0];
        while (ch == ' ' || ch == '\t' || ch == '\n') {
          in = str_drop(in, 1);
          ch = in.data[0];
        }
      } else if (*at == '&') {
        // skip any remaining input
        goto done;
      }

    }
  }
 done:
  va_end(args);
  return true;

 fail:
   va_end(args);
   if(_scan_report_error) {
     fprintf(stderr,
             "str_scan failed,\n   fmt left: '%s'\n input left: '%.*s'\n", at,
             (int)in.len, in.data);
   }
  return false;
}

int str_scan_any(str in, int num_fmt, ...) {
  va_list args;
  va_start(args, num_fmt);
  _scan_report_error = false;
  _scan_array = true;
  va_end(args);
  int matched = -1;
  for (int i = 0; i < num_fmt; i++) {
    const char *fmt = va_arg(args, const char *);
    void **argarr = va_arg(args, void **);
    if (str_scan(fmt, in, argarr)) {
      matched = i;
      goto end;
    }
  }

 end:
  _scan_report_error = true;
  _scan_array = false;
  va_end(args);
  return matched;
}

bool str_splitat(str in, const char *chars, str *split, str *rest) {
  if(in.len == 0) return false;
  size_t at = 0;
  int chs = strlen(chars);
  while(at < in.len) {
    for(int i=0;i<chs;i++) {
      if(in.data[at] == chars[i]) {
        // found split here
        split->len = at;
        split->data = in.data;
        rest->len = in.len - at - 1;
        rest->data = in.data + at + 1;
        return true;
      }
    }
    at++;
  }
  return false;
}


int str_indexof(str in, char ch) {
  for(int i=0;i<in.len;i++) {
    if(in.data[i] == ch) return i;
  }
  return -1;
}

bool str_each_line(str *lines, str *line) {
  if(lines->len == 0) return false;
  if(str_splitat(*lines, "\n", line, lines))
    return true;
  if(lines->len) {
    // last line
    line->len = lines->len;
    line->data = lines->data;
    return true;
  }
  return false;
}

bool is_space(char ch) {
  return ch == ' ' || ch == '\t' || ch == '\n';
}
bool is_digit(char ch) {
  return ch >= '0' && ch <= '9';
}

str str_ltrim(str in) {
  while(in.len && is_space(in.data[0])) {
    in.len--;
    in.data = &in.data[1];
  }
  return in;
}

str str_rtrim(str in) {
  while(in.len && is_space(in.data[in.len-1])) in.len--;
  return in;
}

str str_trim(str in) {
  return str_ltrim(str_rtrim(in));
}

long str_to_long(str s) {
  long l=0;
  long mul = 1;
  if (s.len && s.data[0] == '+') {
    s = str_drop(s, 1);
  } else if(s.len && s.data[0] == '-') {
    mul = -1;
    s = str_drop(s, 1);
  }
  for(size_t i=0;i<s.len;i++) {
    l = (l*10) + (s.data[i]-48);
  }
  return mul * l;
}

long str_to_long_rest(str s, str *rest) {
  long l=0;
  long mul = 1;
  if (s.len && s.data[0] == '+') {
    s = str_drop(s, 1);
  } else if(s.len && s.data[0] == '-') {
    mul = -1;
    s = str_drop(s, 1);
  }
  while(s.len && is_digit(s.data[0])) {
    l = (l*10) + (s.data[0]-48);
    s = str_drop(s, 1);
  }
  *rest = s;
  return mul * l;
}

bool str_startswith(str haystack, str needle) {
  if(needle.len > haystack.len) return false;
  return strncmp(haystack.data,needle.data,needle.len) == 0;
}

str str_drop(str haystack, size_t len) {
  if(len > haystack.len) return (str) {.len = 0, .data=NULL};
  return (str) {.len = haystack.len - len,
                .data = &haystack.data[len]};
}

str str_take(str big, size_t len) {
  if(len > big.len) return big;
  return (str) {.len = len, .data = big.data};
}

str str_from_cstr(const char *in) {
  size_t len = strlen(in);
  char *data = malloc(len);
  if(!data) fprintf(stderr, "Malloc failed!");
  memcpy(data, in, len);
  return (str){.data = data, .len = len};
}

str str_dup(str in) {
  char *data = malloc(in.len);
  if(!data) fprintf(stderr, "Malloc failed!");
  memcpy(data, in.data, in.len);
  return (str) {.len = in.len, .data = data};
}

void str_reverse(str mut) {
  size_t l=0,r=mut.len-1;
  while(l<r) {
    char tmp = mut.data[l];
    mut.data[l] = mut.data[r];
    mut.data[r] = tmp;
    l++; r--;
  }
}

void str_free(str *s) {
  free(s->data);
  s->len = 0;
  s->data = NULL;
}

bool str_eq(str a, str b) {
  if(a.len != b.len) return false;
  return strncmp(a.data, b.data, a.len) == 0;
}

bool str_eq_cstr(str str, const char *cstring) {
  int len = strlen(cstring);
  if(len != str.len) return false;
  return strncmp(str.data, cstring, len) == 0;
}

char str_char_at(str str, size_t idx) {
  if(idx < 0 || idx >= str.len) return 0;
  return str.data[idx];
}

char *str_to_cstr(str str) {
  char *cstr = malloc(str.len+1);
  if(!cstr) {
    printf("malloc failed");
    exit(1);
  }
  memcpy(cstr, str.data, str.len); // FIXME: check
  cstr[str.len] = 0;
  return cstr;
}

/* Allocate new dynamic array of lines extracted */
str *str_lines(str in) {
  str *lines = {0};
  str line;
  str rest = in;
  while(str_splitat(rest, "\n", &line, &rest)) {
    arrput(lines, line);
  }
  if(rest.len > 0)
    arrput(lines, rest);
  return lines;
}

queue initq_(size_t item_size) {
  return (queue){.item_size = item_size,
                 .items = malloc(item_size * 512),
                 .size = 0,
                 .put = 0,
                 .take = 0,
                 .capacity = 512};
}

uint8_t *deq_(queue *q) {
  if (!q->size) {
    fprintf(stderr, "Tried to dequeue from an empty queue!\n");
  }
  uint8_t *ptr = &q->items[q->take * q->item_size];
  q->size--;
  q->take = (q->take + 1) % q->capacity;
  return ptr;
}

void enq_(queue *q, uint8_t *item) {
  if (q->size == q->capacity) {
    size_t new_capacity = q->capacity * 2;
    uint8_t *new = malloc(new_capacity * q->item_size);
    if (!new) {
      fprintf(stderr, "Out of memory, tried to realloc queue to %lld bytes\n",
              q->capacity * q->item_size);
      exit(1);
    }
    uint8_t *old = q->items;
    size_t old_size = q->size;
    size_t old_take = q->take;
    q->size = 0;
    q->take = 0;
    q->put = 0;
    q->items = new;
    q->capacity = new_capacity;
    for (size_t i = 0; i < old_size; i++) {
      enq_(q, &old[((old_take + i) % old_size) * q->item_size]);
    }
    q->take = 0;
    q->put = q->size;
    free(old);
    //printf("new capacity %llu\n", q->capacity);
  }
  memcpy(&q->items[q->put * q->item_size], item, q->item_size);
  q->put = (q->put + 1) % q->capacity;
  q->size++;
}

void freeq(queue *q) {
  q->capacity = 0;
  q->size = 0;
  q->take = 0;
  q->put = 0;
  free(q->items);
}

static inline void _pq_swap(pqueue *pq, size_t a_idx, size_t b_idx) {
  void *a = PQ_PTR(pq, a_idx);
  void *b = PQ_PTR(pq, b_idx);
  memcpy(pq->scratch, a, pq->item_size);
  memcpy(a, b, pq->item_size);
  memcpy(b, pq->scratch, pq->item_size);
}

static inline void _pq_heapify_up(pqueue *pq, size_t idx) {
 start:
  if (idx) {
    void *up = PQ_PTR(pq, (idx-1)/2);
    void *at = PQ_PTR(pq, idx);
    if (pq->compare(up, at) > 0) {
      _pq_swap(pq, (idx - 1) / 2, idx);
      idx = (idx - 1) / 2;
      goto start;
    }
  }
}

void pq_enq_(pqueue *pq, void *item) {
  if (!pq->scratch) {
    pq->scratch = malloc(8192); // alloc 8k for scratch data (for swaps)
    if (!pq->scratch) {
      fprintf(stderr, "Couldn't allocate 8k scratch buffer ¯\\_(ツ)_/¯");
      exit(1);
    }
  }
  if (pq->size == pq->capacity) {
    size_t new_capacity = pq->capacity ? pq->capacity * 2 : 512;
    pq->items = realloc(pq->items, new_capacity * pq->item_size);
    pq->capacity = new_capacity;
    if (!pq->items) {
      fprintf(stderr,
              "Could nor allocate priority queue memory for %zu items (size: "
              "%zu)\n",
              new_capacity, pq->item_size);
      exit(1);
    }
  }

  memcpy(PQ_PTR(pq, pq->size), item, pq->item_size);
  pq->size++;
  _pq_heapify_up(pq, pq->size-1);

}

static inline void _pq_heapify_down(pqueue *pq, size_t idx) {
  size_t smallest, left, right;
  smallest = idx;
 start:
  left = 2 * idx + 1;
  right = 2 * idx + 2;

  if (left < pq->size &&
      pq->compare(PQ_PTR(pq, left), PQ_PTR(pq, smallest)) < 0)
    smallest = left;

  if (right < pq->size &&
      pq->compare(PQ_PTR(pq, right), PQ_PTR(pq, smallest)) < 0)
    smallest = right;

  if (smallest != idx) {
    _pq_swap(pq, smallest, idx);
    idx = smallest;
    goto start;
  }
}


void pq_deq_(pqueue *pq, void *item) {
  if (!pq->size) {
    fprintf(stderr, "Tried to dequeue from empty priority queue\n");
    return;
  }

  memcpy(item, PQ_PTR(pq, 0), pq->item_size);
  pq->size--;
  if(pq->size) {
    memcpy(PQ_PTR(pq, 0), PQ_PTR(pq, pq->size), pq->item_size);
    _pq_heapify_down(pq, 0);
  }
}

void pq_free(pqueue *pq) {
  if (pq->items)
    free(pq->items);
  if (pq->scratch)
    free(pq->scratch);
  pq->size = 0;
  pq->capacity = 0;
}

bool pq_contains_(pqueue *pq, void *item) {
  for (int i = 0; i < pq->size; i++) {
    if(pq->equals(PQ_PTR(pq, i), item)) return true;
  }
  return false;
}

typedef struct _pos { int x, y, steps; } _pos;

long grid4_shortest_distance(void *grid, int width, int height, int from_x,
                             int from_y, int to_x, int to_y,
                             bool (can_move(void *, int, int, int, int))) {

  bool *visited = calloc(width * height, sizeof(bool));
  if (!visited) {
    fprintf(stderr, "Couldn't allocate %dx%d visited buffer\n", width, height);
    exit(1);
  }

  queue q = initq(_pos);
  _pos n = (_pos){.x = from_x, .y = from_y, .steps=0};
  enq(q, n);

  long shortest = 999999999999;
  while (q.size) {
    n = deq(q, _pos);
    if (n.x == to_x && n.y == to_y) {
      //printf("found %d, %d, steps: %d\n", n.x, n.y, n.steps);
      if (n.steps < shortest) {
        shortest = n.steps;
      }
    } else {
      for (int y = n.y - 1; y <= n.y + 1; y++) {
        if(y < 0 || y >= height) continue;
        for (int x = n.x - 1; x <= n.x + 1; x++) {
          if (x < 0 || x >= width)
            continue;
          if (((n.x == x ? 1 : 0) + (n.y == y ? 1 : 0)) != 1)
              continue; // no diagonals or self!
          int idx = (y * width) + x;
          if (!visited[idx] && can_move(grid, n.x, n.y, x, y)) {
            // valid location, enqueue it
            visited[idx] = 1;
            _pos n1 = (_pos){.x = x, .y = y, .steps = n.steps + 1};
            enq(q, n1);
          }
        }
      }
    }
  }
  free(visited);
  freeq(&q);
  return shortest;
}
