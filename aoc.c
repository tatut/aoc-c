#include "aoc.h"

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

bool str_scan(const char *fmt, str in, ...) {
  va_list args;
  va_start(args, in);
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
          long *to = va_arg(args, long*);
          *to = str_to_long_rest(in, &in);
        } else {
          goto fail;
        }
      } else if (*at == '%') {
        // literal %
        if (in.data[0] != '%')
          goto fail;
        in = str_drop(in, 1);
      }
    }
  }
 done:
  va_end(args);
  return true;

 fail:
  va_end(args);
  fprintf(stderr, "str_scan failed,\n   fmt left: '%s'\n input left: '%.*s'\n", at, (int)in.len, in.data);
  return true;
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
