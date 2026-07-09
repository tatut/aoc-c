#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

static inline void tgl_c(long c,uint64_t *t) {
  *t = *t | (uint64_t)1 << c;
  // inspected that these are the only ones actually toggled:
  // the dynamic jumps are not called before being toggled
  // 18: jnz 1 c  -> cpy 1 c
  // 20: jnz 97 d -> cpy 97 d
  // 22: inc d    -> dec d
  // 24: inc c    -> dec c
}

#define TGL(n, normal, toggled) if(!(t & ((uint64_t)1<<n))) { normal } else { toggled }

int main(int argc, char **argv) {
  long a = 12,b = 0, c = 0, d = 0;
  uint64_t t = 0;
  b = a;                                 /* cpy a b */
  b--;                                   /* dec b */
l0: d = a;                               /* cpy a d */
  a = 0;                                 /* cpy 0 a */
 l05: c = b;                             /* cpy b c */
   l1: a++;                              /* inc a */
   c--;                                  /* dec c */
   if(c) goto l1;                        /* jnz c -2 */
   d--;                                  /* dec d */
   if(d) goto l05;                       /* jnz d -5 */
   b--;                                  /* dec b */
   c = b;                                /* cpy b c */
   d = c;                                /* cpy c d */
   l2: d--;                              /* dec d */
   c++;                                  /* inc c */
   if(d) goto l2;                        /* jnz d -2 */
   tgl_c(16+c, &t);                      /* tgl c */
   c = - 16;                             /* cpy -16 c */
   TGL(18, {goto l0;}, { c = 1; })       /* jnz 1 c, always true until toggled */
   c = 75;                               /* cpy 75 c */
   l3: TGL(20, {return 1;}, { d = 97; }) /* jnz 97 d , toggled before execution? */
   l4:a++;                               /* inc a */
   TGL(22, {d++;}, {d--;})               /* inc d */
   if(d) goto l4;                        /* jnz d -2 */
   TGL(24, {c++;}, {c--;})               /* inc c */
   if (c) goto l3;                       /* jnz c -5 */

   printf("A: %ld\n", a);
   return 0;
}
