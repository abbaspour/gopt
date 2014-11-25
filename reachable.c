#include <ctype.h>
#include "gopt.h"
#include "reachable.h"
#include "tools.h"

   
/**
 * Invoked when we see a definition of type DEF TYPE X
 * The difference between this a simple def is that it sets
 * this variable init line to current line number
 * @param id name of current variable
 */
void initial_def(char *id) 
{
   struct defuse *p = first_defuse->next;
   struct defuse *prev = first_defuse;
  
   if(isdigit(id[0]))
      return;
   
   while(p) {
      prev = p;
      p = p->next;
   }
   
   p = (struct defuse*)malloc(sizeof(struct defuse));
   p->defi = 0;
   p->usei = 0;
   p->next = NULL;
   p->initial_line = linenum-1;		/* Voodooo */
   strcpy(p->var, id);
   p->defl[p->defi++] = linenum-1;
   prev->next = p;
   return;
}

/**
 * Invoked when we see a definition 
 * @param id name of current variable
 */
void def(char *id)
{
   struct defuse *p = first_defuse->next;
   struct defuse *prev = first_defuse;
  
   if(isdigit(id[0]))
      return;
   
   while(p) 
   {
      if(!strcmp(p->var, id))
      {
	 p->defl[p->defi++] = linenum-1;
	 return;
      }
      prev = p;
      p = p->next;
   }
   
   p = (struct defuse*)malloc(sizeof(struct defuse));
   p->defi = 0;
   p->usei = 0;
   p->next = NULL;
   strcpy(p->var, id);
   p->defl[p->defi++] = linenum-1;
   prev->next = p;
   return;
}

/**
 * Invoked when we see a usage
 * @param id name of current variable
 */
void use(char *id)
{
   struct defuse *p = first_defuse->next;
   struct defuse *prev = first_defuse;

   if(isdigit(id[0]))
      return;

   while(p) {
      if(!strcmp(p->var, id)) {
	 p->usel[p->usei++] = linenum-1;
	 return;
      }
      prev = p;
      p = p->next;
   }
   p = (struct defuse*)malloc(sizeof(struct defuse));
   p->defi = 0;
   p->usei = 0;
   p->next = NULL;
   strcpy(p->var, id);
   p->usel[p->usei++] = linenum-1;
   prev->next = p;
   return;
}

/**
 * Display def/use in the screen
 */
void print_defuse()
{
   int i;
   struct defuse *p = first_defuse->next;

   printf("def/use chain:\n");
   while(p) {
      printf("variable (%s):\n", p->var);
      printf("\tdef: ");
      for(i = 0; i < p->defi; i++)
	 printf("%d ", p->defl[i]);
      printf("\n");
      printf("\tuse: ");
      for(i = 0; i < p->usei; i++)
	 printf("%d ", p->usel[i]);
      printf("\n");

      p = p->next;
   }
}


/**
 * get contents of line
 * @param indx line number
 */
char *my_getline(int indx)
{
   struct linebuf *p = first_line -> next;
   while(p)
   {
      if(p->num == indx)
	 return p->content;
      p = p->next;
   }
   fprintf(stderr, "Cannot find line content for index = %d\n", indx);
   return "TODO";
}

/**
 * This returns the first occurens of definition of variable v in block b, 
 * which is really important to correntness of some usages(i.e. opt)
 */
int definedin(struct defuse* v, struct block *b, int method)
{
   int i;
   int last = 0;
   
   if(!b)
      return 0;
   
   for(i = 0; i < v->defi; i++)
      if((v->defl[i] >= b->begin) && (v->defl[i] <= b->end))
	 if(method == METHOD_FIRST)
	    return v->defl[i];
	 else 
	    last = v->defl[i];
   return last;
}

/**
 * variable in defuse node v, is used between bl and el 
 */
int usedin(struct defuse* v, struct block *b, int method)
{
   int i;
   int last = 0;
   
   for(i = 0; i < v->defi; i++)
      if((v->usel[i] >= b->begin) && (v->usel[i] <= b->end))
	 if(method == METHOD_FIRST)
	    return v->usel[i];
	 else
	    last = v->usel[i];
   
   return last;
}

/**
 * Find gen for basic block bi
 */
void find_gen(struct block *b)
{
   struct defuse *p = first_defuse->next;
   int line;

   b->defin.geni = 0;

   while(p) {
      if((line = definedin(p, b, METHOD_LAST)) != 0) { 	 /** variable defined in block */
	 b->defin.genl[b->defin.geni] = line;
	 b->defin.genp[b->defin.geni] = p;

	 b->defin.geni++;
      }
      p = p -> next;
   }
}

/**
 * Find kill for basic block b (allways call after find_gen)
 */
void find_kill(struct block *b)
{
   struct block *rest;
   int line;
   int i, j;

   b->defin.killi = 0;

   for(i = 0; i < b->defin.geni; i++)
   {
      rest = first_block;
      //! [1, 2, 3, ...], b, [b+1, b+2, ... , NULL]
      while(rest != b) {
	 if((line = definedin(b->defin.genp[i], rest, METHOD_FIRST)) != 0) {
	    b->defin.killl[b->defin.killi] = line;
	    b->defin.killp[b->defin.killi] = b->defin.genp[i];

	    b->defin.killi++;
	 }
	 rest = rest->next;
      }

      rest = b->next;

      while(rest) {
	 if((line = definedin(b->defin.genp[i], rest, METHOD_FIRST)) != 0) {
	    b->defin.killl[b->defin.killi] = line;
	    b->defin.killp[b->defin.killi] = b->defin.genp[i];

	    b->defin.killi++;
	 }
	 rest = rest->next;
      }

   } /* for i */
}

/**
 * iteratively call find_gen() & find_kill
 */
void calc_genkill()
{
   struct block *b = first_block;
   int i;

   while(b) {
      find_gen(b);
      b = b->next;
   }
   
   b = first_block;
   
   while(b) {
      find_kill(b);
      b = b->next;
   }
}

/**
 * print gen/kill
 */
void print_genkill() 
{
   int i;
   struct block *b = first_block;

   while(b) {
      printf("Gen of block (%s)\n\t", b->id);

      for(i = 0; i < b->defin.geni; i++)
	 printf("%d ", b->defin.genl[i]);

      printf("\nKill of block (%s)\n\t", b->id);

      for(i = 0; i < b->defin.killi; i++)
	 printf("%d ", b->defin.killl[i]);

      printf("\n");

      b = b->next;
   }
}

/**
 * Calculate predecessors
 */
void calc_pred()
{
   struct block *b = first_block;

   while(b) {
      b->predi = 0;
      b = b-> next;
   }

   b = first_block;

   while(b) {
      if(b->to != BLOCK_NONE)
	 b->top->pred[b->top->predi++] = b;

      if(b->to2 != BLOCK_NONE)
	 b->to2p->pred[b->to2p->predi++] = b;

      b = b->next;
   }
}

/**
 * Echo predecessors
 */
void print_pred()
{
   struct block *b = first_block;
   int i;

   while(b) {
      printf("Predesessors of block(%d)[%s]:\n", b->index, b->id);

      for(i = 0; i < b->predi; i++)
	 printf("\t(%d)[%s]\n", b->pred[i]->index, b->pred[i]->id);

      b = b->next;
   }
}


/**
 * Find in/out
 */
void find_inout()
{
   struct block *b = first_block;
   struct block *tmpb = (struct block *) malloc(sizeof(struct block));
   int i;
   int change = 1;
   int oldlen;
   
   calc_pred();
   DEBUG(print_pred());

   while(b) {
      b->defin.ini = 0;
      copy_set(b, OUT_SET, b, GEN_SET);
      b = b->next;
   }

   while(change) {
      change = 0;
      b = first_block;

      while(b) {
	 for(i = 0; i < b->predi; i++)
	    union_set(b, IN_SET, b->pred[i], OUT_SET);

	 copy_set(tmpb, IN_SET, b, IN_SET);
	 sub_set(tmpb, IN_SET, b, KILL_SET);
	 union_set(tmpb, IN_SET, b, GEN_SET);
	 
	 oldlen = b->defin.outi;
	 
	 // According to algorithm I should do what is commented belove
	 /*
	   copy_set(oldb, OUT_SET, b, OUT_SET);
	   copy_set(b, OUT_SET, tmpb, IN_SET);
	 
	   if(!equal_set(oldb, OUT_SET, b, OUT_SET))
	   change = 1;
	 */
	 // but since out[b] = gen[b] we can do
	 // 1. union agains copy
	 // 2. compare set's length to see if it's changed
	 
	 union_set(b, OUT_SET, tmpb, IN_SET);
	 if(oldlen != b->defin.outi) 
	    change = 1;
	 
	 b = b -> next;
      }
   }
}

/**
 * Echo in/out
 */
void print_inout()
{
   struct block *b = first_block;
   int i;
   
   while(b) {
      printf("In of block (%s)\n\t", b->id);

      for(i = 0; i < b->defin.ini; i++)
	 printf("%d ", b->defin.inl[i]);

      printf("\nOut of block (%s)\n\t", b->id);

      for(i = 0; i < b->defin.outi; i++)
	 printf("%d ", b->defin.outl[i]);

      printf("\n");

      b = b->next;
   }
}

/**
 * tryies to guess the last block(termination block)
 */
struct block* find_last_block() {
   int i;
   struct block *t;
   struct block *b = first_block;
   int canbe = 1;// this block still have chance to be the last one

   while(b) {
      t = first_block;
      canbe = 1;
      while(t && canbe) {
	 for(i = 0; i < t->predi; i++)
	    if(t->pred[i] == b) { // no, since this is the father of some one
	       canbe = 0;
	    }
	 t = t -> next;
      }
      if(canbe)
	 return b;
      b = b -> next;
   }
   fprintf(stderr, "No last block!!! this program never ends\n");
   assert(b);
}

/**
 * Main stuff, do optimization
 */
void optimize_reachable() {
   int dc, uc, ic;   /* def, use, in counters */
   int line;			/* line of definition */
   struct block *b, *pblock;
   struct defuse *p = first_defuse->next;
   int use_line, def_line;
   int usefull;
   
   // a defuse is def/line made by while/for
   while(p) {
      DEBUG(printf("Variable %s:\n", p->var));
      for(dc = 0; dc < p->defi; dc++) {
	 pblock = blockfor(p->defl[dc]); // block where this def/line belongs to
	 line = p->defl[dc];
	 DEBUG(printf("Testing agains (%s/%d)\n", pblock->id, line));
	 b = first_block;
	 
	 if(p->initial_line == line) {  // this is -> DEF TYPE XYZ line
	    if(p->usei > 0) { // some one uses this definition
	       DEBUG(printf("\tSome one uses this definition.\n"));
	       continue;
	    } else {
	       DEBUG(printf("\tDefined but not used.\n"));
	       usefull = 0;
	    }
	 } else 
	    while(b) {
	       usefull = 0;
	    
	       if(b == pblock) { // used in local block?
		  for(uc = 0; uc < p->usei; uc++)
		     if((p->usel[uc] > line)  // used after def
			&& (p->usel[uc] <= pblock->end)) { // in this block 
			DEBUG(printf("\tLocaly used.. passing it\n"));
			usefull = 1;
			break;
		     }
	       
		  if(usefull)
		     break;
	       
		  b = b -> next;
		  continue;
	       }
	    
	       for(ic = 0; ic < b->defin.ini; ic++)
		  if(b->defin.inp[ic] == p && b->defin.inl[ic] == line) { // this variable, and this definition
		     DEBUG(printf("\tin of block %s -> ", b->id));
		     use_line = usedin(p, b, METHOD_FIRST);
		     if(use_line == 0)  {
			DEBUG(printf("Not Used\n"));
		     } else  { 
			// although used, but is it used after begin defined
			// definedin() should return the first occurense here, not the last
			def_line = definedin(p, b, METHOD_FIRST);
			if(def_line == 0) {
			   DEBUG(printf("Used(without any other defines)\n"));
			   usefull = 1;
			   break;
			} else if(def_line < use_line) {
			   DEBUG(printf("Not Used(used but defined before)\n"));
			} else {
			   DEBUG(printf("Used(with other defines after it)\n"));
			   usefull = 1;
			}
		     
		     }
		  }
	       if(usefull)
		  break;
	       b = b -> next;
	    }
	 if(!usefull)  {
	    printf("Defination (%s/%d) is USELESS!\n", p->var, line);
	 }
      } // for all lines of definations
      p = p->next;
   } // for all defuses 
}
