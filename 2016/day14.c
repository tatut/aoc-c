#include "../aoc.h"

#include "../vendor/md5-c/md5.h"
#include "../vendor/md5-c/md5.c"
#include <stdint.h>

#include "../stb_ds.h"

inline char hex(uint8_t d) {
  if (d < 10)
    return '0' + d;
  else
    return 'a' + (d-10);
}

#define NIBBLE(md5, x) (0xf & (md5[(x) / 2] >> ((x) % 2 ? 0 : 4)))

// increment printed ascii number
inline void inc(char *start, char **_end) {
  char *end = *_end;
  //printf("num range %p - %p, %c & %c\n", start, end, *start, *end);
  if (*end < '9') {
    *end += 1; // last digit less than 9, just increment that
  } else {
    char *at = end;
    while (at > start && *at == '9')
      at--;
    if (at == start && *at == '9') {
      end++; // make number longer
      *_end = end;
      *at = '1';
    } else {
      *at += 1;
    }
    at++;
    while(at <= end) *(at++) = '0';
  }
}

void rehash(uint8_t *digest, int times) {
  MD5Context init;
  md5Init(&init);

  char as_hex[32];
  while (times--) {
    MD5Context ctx = init;
    for (int n = 0; n < 32; n++) {
      as_hex[n] = hex(NIBBLE(digest, n));
    }
    md5Update(&ctx, (uint8_t *)as_hex, 32);
    md5Finalize(&ctx);
    memcpy(digest, ctx.digest, 16);
  }
}

long part(str salt, int stretch) {
  char hash[64] = {0};
  snprintf(hash, 64, "%.*s", (int)salt.len, salt.data);

  MD5Context salt_ctx;
  md5Init(&salt_ctx);
  md5Update(&salt_ctx, (uint8_t*)hash, salt.len); // init ctx with the salt

  char num[5] = {'0', 0, 0, 0, 0}; // initial number

  long idx = 0;

  // list of waiting indexes by digit 0 - 15
  uint32_t *digits[16] = {0};

  // found indexes
  long found[64] = {0};
  int num_found = 0;

  // we need to loop and insertion sort into the found list
  // because we may find a later index before en earlier index
  // we must loop until the last idx+1000 to ensure no earlier
  // idx is found
  char *start, *end;
  start = end = &num[0];

  while (num_found < 64 || idx < found[63] + 1000) {
    MD5Context ctx = salt_ctx;
    md5Update(&ctx, (uint8_t*)num, (size_t)((end-start)+1));
    md5Finalize(&ctx);

    if(stretch) rehash(ctx.digest, stretch);

    uint8_t *md5 = ctx.digest;
    bool triple = false;
    for (int i = 0; i < 30; i++) { // iterate the hex digits
      // take high or low nibble
      uint8_t d[5];
      d[0] = NIBBLE(md5,i);
      d[1] = NIBBLE(md5,i + 1);
      d[2] = NIBBLE(md5,i + 2);
      if (i <= 27) {
        d[3] = NIBBLE(md5,i + 3);
        d[4] = NIBBLE(md5,i + 4);
        if (d[0] == d[1] && d[0] == d[2] && d[0] == d[3] && d[0] == d[4]) {
          // five same, record success for waiting indexes
          int len = arrlen(digits[d[0]]);
          bool has_same = false;
          for (int i = 0; i < len; i++) {
            if (idx == digits[d[0]][i]) {
              has_same = true;
              continue;
            }
            if (idx - digits[d[0]][i] <= 1000) {
              if(num_found < 64) num_found++;
              long insert_idx = digits[d[0]][i];
              if (num_found == 1) {
                found[0] = insert_idx;
              } else {
                int pos = num_found;
                while (pos >= 1 && (found[pos-1] == 0 || insert_idx < found[pos - 1]))
                  pos--;
                if(pos < 64) {
                  long tmp = found[pos];
                  found[pos] = insert_idx;
                  for (int m = pos + 1; m < num_found; m++) {
                    long tmp1 = found[m];
                    found[m] = tmp;
                    tmp = tmp1;
                  }
                }
              }
            }
          }
          arrsetlen(digits[d[0]], 0);
          // if same line had triple, we can't resolve it yet, add it back
          if (has_same)
            arrpush(digits[d[0]], idx);
        }
      }
      if (!triple && (d[0] == d[1] && d[0] == d[2])) {
        triple = true; // only 1st triple is considered
                       // add this to list of waiting indexes
        arrpush(digits[d[0]], idx);
      }

    }
    idx++;
    inc(start, &end);
  }

  done:
  return found[63];
}

void day14(Day *day) {
  str salt = str_trim(day->input);
  PART1(part(salt, 0));
  PART2(part(salt, 2016));
}
