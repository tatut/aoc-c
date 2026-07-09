#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#define UNREACHABLE() do{printf("should be unreachable: %d", __LINE__); exit(1);}while(0)

long initial_a;
long last_out = 1;
long samples = 0;
static inline bool out(long v) {
  if(last_out == 1 && v != 0) return false;
  if(last_out == 0 && v != 1) return false;
  last_out = v;
  samples++;
  if(samples > 10) {
    printf("%ld\n", initial_a);
    exit(0);
  }
  return true;
}

int main(int argc, char **argv){
  long a=0,b=0,c=0,d=0;
  initial_a = 0;
 start:
  initial_a++;
  a = initial_a;
  b = 0;
  c = 0;
  d = 0;
  last_out = 1;
  samples = 0;
 l00: d = a;          /* cpy a d */
 l01: c = 7;          /* cpy 7 c */
 l02: b = 365;        /* cpy 365 b */
 l03: d++;            /* inc d */
 l04: b--;            /* dec b */
 l05: if(b) goto l03; /* jnz b -2 */
 l06: c--;            /* dec c */
 l07: if(c) goto l02; /* jnz c -5 */
 l08: a = d;          /* cpy d a */
 l09: //NOOP          /* jnz 0 0 */
 l10: b = a;          /* cpy a b */
 l11: a = 0;          /* cpy 0 a */
 l12: c = 2;          /* cpy 2 c */
 l13: if(b) goto l15; /* jnz b 2 */
 l14: goto l20;       /* jnz 1 6 */
 l15: b--;            /* dec b */
 l16: c--;            /* dec c */
 l17: if(c) goto l13; /* jnz c -4 */
 l18: a++;            /* inc a */
 l19: goto l12;       /* jnz 1 -7 */
 l20: b=2;            /* cpy 2 b */
 l21: if(c) goto l23; /* jnz c 2 */
 l22: goto l26;       /* jnz 1 4 */
 l23: b--;            /* dec b */
 l24: c--;            /* dec c */
 l25: goto l21;       /* jnz 1 -4 */
 l26: //NOOP          /* jnz 0 0 */
 l27: if(!out(b)) goto start;         /* out b */
 l28: if(a) goto l09; /* jnz a -19 */
 l29: goto l08;       /* jnz 1 -21 */
}
