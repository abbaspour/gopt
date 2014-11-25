#ifndef _TOOLS_H_
#define _TOOLS_H_

#include "gopt.h"

//! not applicable until the end of parse
struct block* blockfor(int line); 
/** do union  */
void union_set(struct block *dest, int d, struct block *src, int s); 
/** do minus  */
void sub_set(struct block *dest, int d, struct block *src, int s); 

#endif // _TOOLS_H_
