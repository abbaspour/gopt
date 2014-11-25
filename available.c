#include "gopt.h"
#include "available.h"



void zxopy(char *z,  char *x, char *op, char *y)  /* z := x op y */
{
  DEBUG(printf("Operation => %s := %s %s %s => line = %d\n", z, x, op, y, linenum - 1));
}

void copy(char *x, char *y) { // x := y
  DEBUG(printf("Copy => %s := %s  => line = %d\n", x, y, linenum - 1));
}
