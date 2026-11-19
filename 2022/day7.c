#include "../aoc.h"
#include <stdlib.h>

typedef enum file_type { T_FILE, T_DIR } file_type;

typedef struct node {
  str name;
  file_type type;
  long size;
  struct node *parent;

  struct node *children; // first child
  struct node *next; // next sibling
} node;

node *child(node *parent, str name) {
  // get existing child
  node *c = parent->children;
  while (c) {
    if (str_eq(c->name, name))
      return c;
    c = c->next;
  }

  node *n = malloc(sizeof(node));
  if (!n) {
    fprintf(stderr, "Get more memory ¯\\_(ツ)_/¯\n");
    exit(1);
  }
  *n = (node){.name = name};
  n->parent = parent;
  if (parent->children) {
    n->next = parent->children;
    parent->children = n;
  } else {
    parent->children = n;
  }
  return n;
}

void parse(str *lines, node *at) {
  str line;
  node *d;
  while (str_each_line(lines, &line)) {
    if(!line.len) return;
    if (str_startswith(line, str_constant("$ cd "))) {
      line = str_drop(line, 5);
      if (str_eq(line, str_constant(".."))) {
        at = at->parent;
      } else {
        d = child(at, line);
        d->type = T_DIR;
        d->size = -1;
        parse(lines, d);
      }
    } else if (str_eq(line, str_constant("$ ls"))) {
      // read file listing for current directory until line starting with '$'
      while (lines->len && str_char_at(*lines, 0) != '$') {
        str_each_line(lines, &line);
        if (str_startswith(line, str_constant("dir "))) {
          d = child(at, str_drop(line, 4));
          d->type = T_DIR;
          d->size = -1;
        } else {
          long sz;
          str name;
          if (!str_scan("%ld %s", line, &sz, &name))
            goto fail;
          d = child(at, name);
          d->type = T_FILE;
          d->size = sz;
        }
      }
    }
  }
  return;
fail:
  fprintf(stderr, "Bad line: %.*s\n", (int)line.len, line.data);
  exit(1);
}


long dir_size(node *dir) {
  if (dir->size != -1)
    return dir->size; // already calculated

  long size = 0;
  node *at = dir->children;
  while (at) {
    if (at->type == T_FILE) {
      size += at->size;
    } else {
      long ds = dir_size(at);
      size += ds;
    }
    at = at->next;
  }
  dir->size = size;
  return size;
}


void dirs_under_100k(node *at, long *sum) {
  if (at->type == T_DIR) {
    long sz = dir_size(at);
    if (sz <= 100000)
      *sum += sz;
    at = at->children;
    while (at) {
      dirs_under_100k(at, sum);
      at = at->next;
    }
  }
}

void smallest_to_delete(node *at, long need, long *smallest) {
  if (at->type == T_DIR) {
    long sz = dir_size(at);
    if (sz >= need) {
      if(sz < *smallest) *smallest = sz;
    }
    at = at->children;
    while (at) {
      smallest_to_delete(at, need, smallest);
      at = at->next;
    }
  }
}

void day7(Day *day) {
  str lines = day->input, line;
  node root = {.type = T_DIR, .name = str_constant("/"), .size=-1};
  str_each_line(&lines, &line); // skip first line, we are at root
  parse(&lines, &root);

  long part1 = 0;
  dirs_under_100k(&root, &part1);
  PART1(part1);

  long part2 = 999999999;
  long need = 30000000 - (70000000 - dir_size(&root));
  smallest_to_delete(&root, need, &part2);
  PART2(part2);
}
