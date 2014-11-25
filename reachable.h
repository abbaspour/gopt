#ifndef _REACHABLE_H_
#define _REACHABLE_H_


extern struct block *first_block, *current_block;

void def(char *);
void use(char *);

void print_defuse();
void calc_genkill();
void find_inout();
void optimize_reachable();

#endif //_REACHABLE_H_
