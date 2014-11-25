#include "gopt.h"

int linenum = 1;		/**< line number in code.3a file, we currently visit  */
struct block *first_block;	/**< first block of basic-block's list  */
struct block *current_block;	/**< basic block we are currently visiting  */
short lastid = 0;		/**< an increasing number to give unique block numbering */
struct linebuf *first_line;	/**< pointer to first line of cached source. \bug starts from next ;) */
struct linebuf *current_line;	/**< pointer to current line of cached source */
struct defuse *first_defuse;	/**< pointer to first def/use chain element  */
int after_goto = 0;		/**< set to 1 whenever we visit goto, so no link is set from prevoius
				 block to this block*/
/**
 * Global Optimizer Initiator
 * This function initializes the global optimizer
 * we set diffrent values such as fist_block, etc...
 */
void init()
{
   first_block = (struct block*) malloc(sizeof(struct block));
   first_block -> begin = 1;
   first_block -> to   = BLOCK_NONE;
   first_block -> to2  = BLOCK_NONE;
   first_block -> top  = NULL;
   first_block -> to2p = NULL;
   first_block -> index = lastid++;
   strcpy(first_block->id, "start");
   first_block -> next = NULL;
   current_block = first_block;

   first_line = (struct linebuf*) malloc(sizeof(struct linebuf));
   first_line -> num = 0;
   current_line = first_line;

   first_defuse = (struct defuse*) malloc(sizeof(struct defuse));
   first_defuse->defi = 0;
   first_defuse->usei = 0;
   first_defuse -> next = NULL;
}

/**
 * Prints basic blocks
 * This functions gives a text based representation of basic blocks
 * @see draw_blocks()
 */
void print_blocks()
{
   struct block *b = first_block;
   
   while(b) {
      printf("block(%d)[%s] from %d to ", b->index, b->id, b->begin);
      if(b->end != LINE_END)
	 printf("%d connect", b->end);
      else
	 printf("END connect", b->to);
      
      if(b->to != BLOCK_NONE)
      {
	 printf("(%d", b->to);
	 
	 if(b->to2 != BLOCK_NONE)
	    printf(", %d)", b->to2);
	 else
	    printf(")");
      }
      printf("\n");
      
      b = b->next;
   }
   
}

/**
 * Draw basic blocks graphicaly
 * This function does same as print_blocks but, produces DOT based 
 * output file.
 * @see print_blocks()
 */
void draw_blocks()
{
   FILE *outp;
   int i;
   struct block *b = first_block;
   
   outp = fopen("basic-block.dot", "w+");
   if(outp == NULL) {
      fprintf(stderr, "Cannot open output file to draw basic blocks!\n");
      exit(EXIT_FAILURE);
   }
   
   fprintf(outp, "/* output from optimizer over mesi 3 address code */\n");
   fprintf(outp, "digraph \"Basic Blocks\" { \n");
   fprintf(outp, "\tnode[/*style=filled, color=gray92,*/ shape=box];\n");
   
   while(b) {
      fprintf(outp, "\t%d[shape=record,label=\"{%s| gen: ", b->index, b->id);
      
      for(i = 0; i < b->defin.geni; i++)
	 fprintf(outp, "%s(%d) ", b->defin.genp[i]->var, b->defin.genl[i]);
      
      fprintf(outp, "| kill: ");
      
      for(i = 0; i < b->defin.killi; i++)
	 fprintf(outp, "%s(%d) ", b->defin.killp[i]->var, b->defin.killl[i]);
      
      fprintf(outp, "| in: ");
      
      for(i = 0; i < b->defin.ini; i++)
	 fprintf(outp, "%s(%d) ", b->defin.inp[i]->var, b->defin.inl[i]);
      
      fprintf(outp, "| ");
      
      for(i = b->begin; i <= (b->end == LINE_END ? linenum - 1: b->end); i++)
	 fprintf(outp, "%d: %s \\l", i, my_getline(i));
      
      fprintf(outp, "| out: ");
      
      for(i = 0; i < b->defin.outi; i++)
	 fprintf(outp, "%s(%d) ", b->defin.outp[i]->var, b->defin.outl[i]);
      
      fprintf(outp, "}\"];\n");
      
      if(b->to != BLOCK_NONE)
	 fprintf(outp, "\t%d->%d;\n", b->index, b->to);
      
      if(b->to2 != BLOCK_NONE)
	 fprintf(outp, "\t%d->%d;\n", b->index, b->to2);
      
      b = b->next;
   }

   fprintf(outp, "}\n");
   fclose(outp);
}

/**
 * Print input code's lines
 * This function prints input file's lines on the screen.
 */
void print_lines()
{
   struct linebuf *l = first_line->next;
   
   while(l) {
      printf("line %d -> %s\n", l->num, l->content);
      l = l->next;
   }
}

/**
 * Add command to code cache
 * This function adds a 1 line command to code cache, and increases the linenum
 * @param p a pointer to command string
 * @see linenum
 * @see linebuf
 * @return pointer to added linebuf struct
 */
struct linebuf *mk_cmd(char *p)
{
   struct linebuf *l = (struct linebuf*) malloc(sizeof(struct linebuf));
   char *content = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
   sprintf(content, "%s", p);

   l->content = content;
   l->num = linenum++;
   return l;
}

/**
 * Add command to code cache
 * This function adds a 2 line command to code cache, and increases the linenum
 * @param p a pointer to command string, part 1
 * @param p2 a pointer to command string, part 2
 * @see linenum
 * @see linebuf
 * @return pointer to added linebuf struct
 */
struct linebuf *mk_cmd2(char *p, char *p2)
{
   struct linebuf *l = (struct linebuf*) malloc(sizeof(struct linebuf));
   char *content = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
   sprintf(content, "%s\t%s", p, p2);

   l->content = content;
   l->num = linenum++;
   return l;
}

/**
 * Add command to code cache
 * This function adds a 3 line command to code cache, and increases the linenum
 * @param p a pointer to command string, part 1
 * @param p2 a pointer to command string, part 2
 * @param p3 a pointer to command string, part 3
 * @see linenum
 * @see linebuf
 * @return pointer to added linebuf struct
 */
struct linebuf *mk_cmd3(char *p, char *p2, char *p3)
{
   struct linebuf *l = (struct linebuf*) malloc(sizeof(struct linebuf));
   char *content = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
   sprintf(content, "%s\t%s\t%s", p, p2, p3);

   l->content = content;
   l->num = linenum++;
   return l;
}

/**
 * Add command to code cache
 * This function adds a 4 line command to code cache, and increases the linenum
 * @param p a pointer to command string, part 1
 * @param p2 a pointer to command string, part 2
 * @param p3 a pointer to command string, part 3
 * @param p4 a pointer to command string, part 4
 * @see linenum
 * @see linebuf
 * @return pointer to added linebuf struct
 */
struct linebuf *mk_cmd4(char *p, char *p2, char *p3, char *p4)
{
   struct linebuf *l = (struct linebuf*) malloc(sizeof(struct linebuf));
   char *content = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
   sprintf(content, "%s\t%s\t%s\t%s", p, p2, p3, p4);

   l->content = content;
   l->num = linenum++;
   return l;
}

/**
 * Add command to code cache
 * This function adds a 4 line command to code cache, and increases the linenum
 * @param p a pointer to command string, part 1
 * @param p2 a pointer to command string, part 2
 * @param p3 a pointer to command string, part 3
 * @param p4 a pointer to command string, part 4
 * @param p5 a pointer to command string, part 5
 * @see linenum
 * @see linebuf
 * @return pointer to added linebuf struct
 */
struct linebuf *mk_cmd5(char *p, char *p2, char *p3, char *p4, char *p5)
{
   struct linebuf *l = (struct linebuf*) malloc(sizeof(struct linebuf));
   char *content = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
   if(!strcmp(p, "if"))
      sprintf(content, "%s\t%s\t%s\t%s\tgoto\t%s", p, p2, p3, p4, p5);
   else
      sprintf(content, "%s\t%s\t%s\t%s\t%s", p, p2, p3, p4, p5);

   l->content = content;
   l->num = linenum++;
   return l;
}

/**
 * Add label to code cache
 * This function adds a label line to code cache, and increases the linenum
 * @param p a pointer to label string 
 * @see linenum
 * @see linebuf
 * @return pointer to added linebuf struct
 */
struct linebuf *mk_label(char *p)
{
   struct linebuf *l = (struct linebuf*) malloc(sizeof(struct linebuf));
   char *content = (char *) malloc(sizeof(char) * (strlen(p)+2));
   sprintf(content, "%s\t:", p);
   
   l->content = content;
   l->num = linenum++;
   return l;
}

/**
 * Incude a linebuf to code cache
 * @param l a linebuf pointer to a new line
 */
void add_buf(struct linebuf *l)
{
   current_line -> next = l;
   l -> next = NULL;
   current_line = l;
   return;
}

/**
 * Returns a block with given id
 * @param id id of asked block
 * @return block pointer if found, otherwise NULL
 */
struct block *fetch_block(char *id)
{
   struct block *b = first_block;

   while(b)
   {
      if(!strcmp(b->id, id))
	 return b;
      b = b->next;
   }
   return NULL;
}

/**
 * Goto to the last block from basic-blocks list
 * @return last basic-block
 */
struct block* gotoend() {
  struct block* b = first_block;
  while(b->next) 
    b  = b -> next;
  return b;
}

/**
 * Invoked when we see a label in the code
 * Invoked when we see a label in the code, hence basic block structure have to be changed
 * @param l label name found
 */
void found_label(char *l)
{
   struct block *new_block;
   short found = 1;

   // if after goto current is g not here, but this is not the case all times
   // since it can be after goto, but current be here(when g is allocated before) 

   if(!after_goto)
      current_block->end = linenum - 2;

   // we have seen this, or there was a 'goto x' where this label comes after it. 
   if((new_block = fetch_block(l)) == NULL) { // no such a thing
      found = 0;
      new_block =  (struct block*) malloc(sizeof(struct block));
      new_block->index = lastid++;
      new_block->next = NULL;
      new_block->end = LINE_END;
      new_block->to = BLOCK_NONE;
      new_block->to2 = BLOCK_NONE;
      sprintf(new_block->id, "%s", l);
   }

   new_block->begin = linenum - 1;

   if(!after_goto) {
      current_block->to = new_block->index;
      current_block->top = new_block;
   } else {
      after_goto = 0;
   }

   current_block->to2 = BLOCK_NONE;
   current_block->to2p = NULL;

   current_block = gotoend();

   if(!found) {
      current_block->next = new_block;
   }
   current_block = new_block;
}

/**
 * Invoked when we see a if in the code
 * Invoked when we see a if in the code, hence basic block structure have to be changed
 * @param i if name found(is where if goes)
 */
void found_if(char *i)
{
   struct block *cont_block =  (struct block*) malloc(sizeof(struct block)); /* Continued Block (!cond)*/
   struct block *go_block; /* Goto Block(cond) */
   short found = 1;

   if(!after_goto)
      current_block->end = linenum - 1;

   cont_block->begin = linenum;
   cont_block->end = LINE_END;
   cont_block->index = lastid++;
   cont_block->to = BLOCK_NONE;
   cont_block->to2 = BLOCK_NONE;
   sprintf(cont_block->id, "_if_%s", i);

   current_block->to = cont_block->index;
   current_block->top = cont_block;

   if((go_block = fetch_block(i)) == NULL) {
      found = 0;
      go_block =  (struct block*) malloc(sizeof(struct block));
      go_block->index = lastid++;
      go_block->next = NULL;
      go_block->end = LINE_END;
      go_block->to = BLOCK_NONE;
      go_block->to2 = BLOCK_NONE;
      sprintf(go_block->id,"%s", i);
   }

   current_block->to2 = go_block->index;
   current_block->to2p = go_block;

   current_block = gotoend();
   if(!found) {
      current_block->next = go_block;
      current_block = go_block;
   }
   current_block->next = cont_block;
   current_block = cont_block;
}

/**
 * Invoked when we see a goto in the code
 * Invoked when we see a goto in the code, hence basic block structure have to be changed
 * Note that goto differs from if/label, since it is a terminator, not a beginner of basic-blocks.
 * @param g goto name found(is where goto goes)
 */
void found_goto(char *g)
{
   short found = 1;
   // where goto g goes
   struct block *label_block; 
   struct block *swap_block; 
   // where goto g continues, there should be a label, to aviod unreachable code

   if(!after_goto)
      current_block->end = linenum - 1;

   if((label_block = fetch_block(g)) == NULL)
   {
      found = 0;
      label_block =  (struct block*) malloc(sizeof(struct block));
      label_block->index = lastid++;
      label_block->next = NULL;
      label_block->end = LINE_END;
      label_block->to = BLOCK_NONE;
      label_block->to2 = BLOCK_NONE;
      sprintf(label_block->id,"%s", g);
   }

   current_block->to = label_block->index;
   current_block->top = label_block;

   current_block->to2 = BLOCK_NONE;
   current_block->to2p = NULL;

   // goto differs from if/label, cuz goto is finished not starter
   current_block->end = linenum - 1;

   after_goto = 1;
   
   current_block = gotoend();

   if(!found) {
      current_block->next = label_block;
      current_block = label_block;
      return;
   } 
}
