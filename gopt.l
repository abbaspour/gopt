/* lex specification for Mesi output */

%{
#include <string.h>
  static int line_no = 1;
%}

ID [A-Za-z_][A-Za-z0-9]*

%%

"if"     { return tIF; }
"def"    { return tDEF; }
":="     { return tASSIGN; }
"goto"   { return tGOTO; }
"not"    { return tNOT; }
":"	 { return tCOLON; }
"int"    { yylval.idval = strdup(yytext); return tINT; }
"char"   { yylval.idval = strdup(yytext); return tCHAR; }
"+"      { return tADD; }
"-"      { return tSUB; }
"*"      { return tMUL; }
"/"      { return tDIV; }
"="      { return tEQU; }
"<"      { return tLESS; }
">"      { return tMORE; }
"param"  { return tPARAM; }
"call"   { return tCALL; }

{ID}     { yylval.idval = strdup(yytext); return tIDENTIFIER; }
[0-9]+   { yylval.numval = atoi(yytext); return tNUMBER; }
[ \t]  /* ignore whitespace */
.        { return *yytext; }
\n       { line_no++;}
%%
