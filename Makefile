CC = gcc -g -w
YACC = yacc -y
LIBS = -lfl
LEX = flex 

OBJS =  gopt.o available.o reachable.o tools.o syntax.o

all:	goptl syntax gopt 

goptl: gopt.l
	$(LEX) gopt.l

syntax: syntax.y
	$(YACC) syntax.y

gopt:	$(OBJS)
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

clean:	
	rm -f *.o gopt y.tab.c lex.yy.c *.ps *.aux *.dvi *.log *.tex *.ftx *~ *.dot *.ps

TAGS: 
	etags *.c *.h

document: document.fvx
	fvx2ftx -f document.fvx document.ftx
	ftex document.ftx
	rm document.ftx
	
doxygen: Doxyfile
	doxygen
