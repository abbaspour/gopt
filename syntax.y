%{
#include <stdio.h>
#include "gopt.h"
#include "reachable.h"
#include "available.h"
#include "tools.h"
%}

%union {
  int numval;
  char *idval;
  struct linebuf* buf;
}

%token tIF
%token tIF_NOT
%token tDEF
%token tMOV
%token tASSIGN
%token tGOTO
%token tNOT
%token tCOLON
%token tINT
%token tCHAR
%token tADD
%token tSUB
%token tMUL
%token tDIV
%token tEQU
%token tLESS
%token tMORE
%token tCALL
%token tPARAM

%token <idval> tIDENTIFIER 
%token <numval> tNUMBER
%type  <buf> line command label assign_cmd copy_cmd goto_cmd if_cmd param_cmd call_cmd def_cmd 
%type  <idval> type variable op relop

%start program

%%
program : lines {
  DEBUG(print_blocks()); 
  DEBUG(print_lines()); 
  DEBUG(print_defuse()); 
  calc_genkill();
  find_inout(); 
  DEBUG(print_inout()); 
  optimize_reachable(); 
#ifdef DOT_OUTPUT
  draw_blocks(); 
#endif
}
;

lines : line lines {}
| line{}
;

line: command{add_buf($1);}
    | label{add_buf($1);}
;

command: assign_cmd {$$ = $1;}
       | copy_cmd { $$ = $1; }
       | goto_cmd {$$ = $1;}
       | if_cmd {$$ = $1;}
       | param_cmd { $$ = $1; }
       | call_cmd { $$ = $1; }
       | def_cmd {$$ = $1;}
;

label: tIDENTIFIER tCOLON {$$ = mk_label($1);found_label($1);}
;

assign_cmd: tIDENTIFIER tASSIGN variable op variable {$$ = mk_cmd5($1, ":=", $3, $4, $5); def($1); use($3); use($5); zxopy($1, $3, $4, $5);}
; 	  

copy_cmd: tIDENTIFIER tASSIGN variable { $$ = mk_cmd3($1, ":=", $3); def($1); use($3); copy($1, $3);};
;

/* there should allways be a lable after goto... handle it some how */
goto_cmd: tGOTO tIDENTIFIER {$$ = mk_cmd2("goto", $2);found_goto($2);}
;

if_cmd: tIF variable relop variable tGOTO tIDENTIFIER { $$ = mk_cmd5("if", $2, $3, $4, $6); found_if($6); use($2); use($4);}
;

param_cmd: tPARAM variable { $$ = mk_cmd2("param", $2); use($2); }
;

call_cmd: tCALL tIDENTIFIER tNUMBER {char num[5]; sprintf(num , "%d", $3); $$ = mk_cmd3("call", $2, num); }
;

def_cmd: tDEF type tIDENTIFIER { $$ = mk_cmd3("def", $2, $3); initial_def($3);}
; 

op: tADD   { $$ = "+"; }
      | tSUB { $$ = "-"; }
      | tMUL { $$ = "*"; }
      | tDIV { $$ = "/"; }
;

relop: tLESS { $$ = "<"; }
| tMORE { $$ = "\\>"; /* cuz dot cannot display single '>' :) */}
| tEQU { $$ = "="; }
;

type: tINT { $$ = "int";}
      | tCHAR { $$ = "char"; }
;

variable: tNUMBER { $$ = (char*) malloc(5*sizeof(char)); sprintf($$ , "%d", $1); }
| tIDENTIFIER { $$ = $1; }
;

%%
int main()
{
  init();
  yyparse();
}

#include "lex.yy.c"

int yyerror(char *s)
{
  fprintf(stderr, "%s[%s] at line %d\n", s, yytext, line_no);
  return 0;
}
