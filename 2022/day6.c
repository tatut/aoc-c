#include "../aoc.h"
void day6(Day *day){
  str buf = day->input;
  bool part1 = false, part2 = false;
  for(int p=3; p<buf.len;p++) {

    if(!part1) {
      char *sop = &buf.data[p-3];
      if(sop[0] != sop[1] && sop[0] != sop[2] && sop[0] != sop[3] &&
         sop[1] != sop[2] && sop[1] != sop[3] &&
         sop[2] != sop[3]) {
        PART1(p+1);
        part1 = true;
      }
    }
    if(!part2 && p >= 13) {
      char *sop = &buf.data[p-13];
      bool found = true;
      for(int i=0;i<14;i++) {
        for(int j=i+1;j<14;j++) {
          if(sop[i]==sop[j]) {
            found = false;
            goto done;
          }
        }
      }
    done:
      if(found) {
        PART2(p+1);
        part2 = true;
      }
    }
    if(part1 && part2) break;
  }
}
