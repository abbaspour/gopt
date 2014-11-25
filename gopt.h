#ifndef __GOPT_H__
# define __GOPT_H__

# define BLOCK_NONE -1		/**< pointer to a NULL block  */
# define LINE_END  10000	/**< pointer to end of program.
				   should be a BIG number. \todo Find a cool value */

# define MAX_VAR_NAME  20	/**< maximum variable name  */

# define MAX_DEF_LINES 100	/**< maximum lines of definition */
# define MAX_USE_LINES 100	/**< maximum lines of use */

# define MAX_IN_LINES   20	/**< maximum number of Ins for each block  */
# define MAX_OUT_LINES  20	/**< maximum number of Outs for each block  */
# define MAX_KILL_LINES 20	/**< maximum number of Kills for each block  */
# define MAX_GEN_LINES  20	/**< maximum number of Gens for each block  */

# define MAX_PRED 10		/**< maximum predecessors of each block  */
# define MAX_LINE_LEN 80	/**< maximum input code line length  */

# define MAX_LINE  10000	/**< maximum lines of code in input file */

# include <stdio.h>
# include <stdlib.h>
# include <assert.h>

# define ASSERT(x) assert(x)	/**< Do we want assestion?  */
# define DEBUG(x)		/**< Do we want debuf?  */
# define DOT_OUTPUT		/**< Do we want DOT output? \todo not used yet  */

enum {IN_SET = 0, OUT_SET, KILL_SET, GEN_SET};
enum {OP_NOP = 0, OP_ADD, OP_SUB, OP_MUL, OP_DIV, OP_NEG, OP_LESS, OP_MORE};

/**
 * how to search for def/use in blocks. return the first occurent line, or the last
 */
enum  { METHOD_FIRST = 0, METHOD_LAST }; 

extern int linenum;

/**
 * Raw definations holder. 
 * used later to find where each def belongs to.
 * This is raw, cuz we dono where each one belongs to. After completing the calculation
 * of basic-blocks, we use this to set definations
 */
struct defuse
{
  char var[MAX_VAR_NAME];
  int  defl[MAX_DEF_LINES];	/** \todo should be dynamic*/
  int  defi;
  int  usel[MAX_USE_LINES];	/** \todo should be dynamic  */
  int  usei;
  int  initial_line;
  struct defuse *next;
};
extern struct defuse *first_defuse;

/**
 * structure used for available-expression analysis 
 */
struct expression
{
  int operation;
  char l[MAX_VAR_NAME];
  char r[MAX_VAR_NAME];
};

/**
 * Definations(in/out/gen.kill) in each block
 */
struct definition
{
  int inl[MAX_IN_LINES];	        /**< lines of in */
  struct defuse *inp[MAX_IN_LINES];     /**< pointers of in */
  int ini;			        /**< index of in */

  int outl[MAX_OUT_LINES];	        /**< lines of out  */
  struct defuse *outp[MAX_OUT_LINES];   /**< pointers of out */
  int outi;			        /**< index of out  */

  int killl[MAX_KILL_LINES];	        /**< lines of kill  */
  struct defuse *killp[MAX_KILL_LINES]; /**< pointers of kill */
  int killi;			        /**< index of kill  */

  int genl[MAX_GEN_LINES];	        /**< lines of gen  */
  struct defuse *genp[MAX_GEN_LINES];   /**< pointers of gen */
  int geni;			        /**< index of gen  */
};

/**
 * Expressions (gen/kill/in/out) in each block
 */
struct available_expr
{
  int inl[MAX_IN_LINES];	        /**< lines of in */
  struct defuse *inp[MAX_IN_LINES];     /**< pointers of in */
  int ini;			        /**< index of in */

  int outl[MAX_OUT_LINES];	        /**< lines of out  */
  struct defuse *outp[MAX_OUT_LINES];   /**< pointers of out */
  int outi;			        /**< index of out  */

  int killl[MAX_KILL_LINES];	        /**< lines of kill  */
  struct defuse *killp[MAX_KILL_LINES]; /**< pointers of kill */
  int killi;			        /**< index of kill  */

  int genl[MAX_GEN_LINES];	        /**< lines of gen  */
  struct defuse *genp[MAX_GEN_LINES];   /**< pointers of gen */
  int geni;			        /**< index of gen  */
};

struct block
{
  int index;			        /**< block unique number  */
  char  id[20];			        /**< block name  */
  short begin, end;		        /**< begin & end lines of block*/
   
  short to, to2;	 	        /**< where we go from here(their indexes)  */
  struct block *top, *to2p;	        /**< where we go from here(their poniters)  */

  struct definition defin;	        /**< reaching-defination in/out/kill/gen */
  struct available_expr expr;           /**< available-expression in/out/kill/gen */

  struct block *pred[MAX_PRED];         /**< predesessors of block */
  int predi;			        /**< predesessor index */
   
  struct block *next;		        /**< next(in memory) block */
};

extern struct block *first_block, *current_block;
extern short lastid;

/**
 * Code cache holder
 */
struct linebuf
{
  char *content;		/**< content string */
  int num;			/**< line number  */
  struct linebuf* next;		/**< next(in mem & reality) line */
};

extern struct linebuf *first_line, *current_line;

void init();
void print_blocks();
struct linebuf *mk_cmd(char *);
struct linebuf *mk_cmd2(char *, char *);
struct linebuf *mk_cmd3(char *, char *, char *);
struct linebuf *mk_label(char *);
void add_buf(struct linebuf *);

void found_label(char *);
void found_if(char *);
void found_goto(char *);

#endif //  __GOPT_H__
