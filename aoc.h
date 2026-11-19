#ifndef aoc_h
#define aoc_h

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef struct str {
  size_t len;
  char *data;
} str;

typedef struct queue {
  uint64_t capacity;
  uint64_t size; // how many items
  uint64_t take; // take pos
  uint64_t put;  // put pos
  size_t item_size;
  uint8_t *items;
} queue;

typedef enum AnswerType {
  NO_ANSWER = 0,
  LONG_ANSWER = 1,
  STR_ANSWER = 2
} AnswerType;

typedef struct Answer {
  AnswerType type;
  union {
    long long_answer;
    str str_answer;
  } answer;
} Answer;

typedef struct Day {
  int year;
  int day;
  str input;
  Answer part1;
  Answer part2;
  clock_t start;
  clock_t end;
} Day;

typedef enum dir4 {
  UP, RIGHT, DOWN, LEFT
} dir4;

void set_long_answer(Answer *a, long ans);
void set_str_answer(Answer *a, str ans);

#define ANSWER(part, x) _Generic((x), \
                                 long: set_long_answer(part, x),       \
                                 int: set_long_answer(part, (int)(x)))

#define PART1(x) ANSWER(&day->part1, (x))
#define PART2(x) ANSWER(&day->part2, (x))

#define str_constant(X) ((str){.len=sizeof(X)-1, .data=X})

bool str_splitat(str in, const char *chars, str *split, str *rest);
int str_indexof(str in, char ch);
bool str_each_line(str *lines, str *line);
bool is_space(char ch);
bool is_digit(char ch);
str str_ltrim(str in);
str str_rtrim(str in);
str str_trim(str in);
long str_to_long(str s);
long str_to_long_rest(str s, str *rest);
bool str_startswith(str haystack, str needle);
str str_drop(str haystack, size_t len);
str str_take(str big, size_t len);
str str_from_cstr(const char *in);
str str_dup(str in);
void str_reverse(str mut);
void str_free(str *s);
bool str_eq(str a, str b);
bool str_eq_cstr(str str, const char *cstring);
char str_char_at(str str, size_t idx);
char *str_to_cstr(str str);
str *str_lines(str in);
bool str_scan(const char *fmt, str in, ...);
int str_scan_any(str in, int num_fmt, ...);

queue initq_(size_t item_type);
void enq_(queue *q, uint8_t *item);
uint8_t *deq_(queue *q);
void freeq(queue *q);
#define initq(type) initq_(sizeof(type))
#define enq(q, item) enq_(&(q), (uint8_t*)(&item))
#define deq(q, type) *((type*)deq_(&(q)))

#define CMP_FN(name, type, field)                                              \
  int name(const void *aptr, const void *bptr) {                               \
    type *a = (type *)aptr;                                                    \
    type *b = (type *)bptr;                                                    \
    if (a->field < b->field)                                                   \
      return -1;                                                               \
    if (a->field > b->field)                                                   \
      return 1;                                                                \
    return 0;                                                                  \
  }

#define CMP_FN_REV(name, type, field) int name(const void *aptr, const void *bptr) { \
    type *a = (type*)aptr; \
    type *b = (type*)bptr; \
    if(b->field < a->field) return -1; \
    if(b->field > a->field) return 1; \
    return 0; \
    }

#define EQ_FN(name, type, expr)                                                \
  bool name(const void *aptr, const void *bptr) {                              \
    type *a = (type *)aptr;                                                    \
    type *b = (type *)bptr;                                                    \
    if (expr)                                                                  \
      return true;                                                             \
    return false;                                                              \
  }

typedef struct pqueue {
  int (*compare)(const void *, const void *);
  bool (*equals)(const void*, const void*);
  size_t capacity;
  size_t size;
  size_t item_size; // size of a single item
  void *items;
  char *scratch; // scratch data to use for temp swaps
} pqueue;

#define PQ_PTR(pq, n)                                                          \
  ((void *)((char *)(pq)->items + ((n) * (pq)->item_size)))

#define pq_enq(pq, item) pq_enq_(&(pq), (void *)(&item))
#define pq_deq(pq, to) pq_deq_(&(pq), (void*)&(to))
#define pq_init(type, cmp_fn, eql_fn)                                          \
  (pqueue) { .item_size = sizeof(type), .compare = cmp_fn, .equals = eql_fn }
#define pq_contains(pq, item) pq_contains_(&(pq), (void *)&(item))

void pq_enq_(pqueue *pq, void *item);
void pq_deq_(pqueue *pq, void *item);
void pq_free(pqueue *pq);
bool pq_contains_(pqueue *pq, void *item);

/**
 * Calculate BFS shortest distance on a (not huge) UP,DOWN,LEFT,RIGHT grid.
 * Allocates a bitset to hold visited.
 *
 * The grid is any user defined pointer, that is passed to callbacks.
 */
long grid4_shortest_distance(void *grid,
                int width, int height, int from_x, int from_y, int to_x,
                             int to_y, bool (can_move(void*,int,int,int,int)));

u8 popcount(u64 n);

#endif
