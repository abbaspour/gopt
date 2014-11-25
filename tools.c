#include "tools.h"

// not applicable until the end of parse
struct block* blockfor(int line) 
{
  struct block *b = first_block;
  while(b) 
    {
      if((line >= b->begin) && (line <= (b->end != LINE_END ? b->end : MAX_LINE)))
	return b;
	b = b -> next;
    }
  printf("Dono where line[%d] belongs.. this should not happend!\n", line);
  ASSERT(b); 
}

int equal_set(struct block *dest, int d, struct block *src, int s) {
   int *dlp;
   struct defuse **dp;
   int *di;

   int *slp;
   struct defuse **sp;
   int *si;

   int i, j;
   int found;
   int isthere;
   
   fprintf(stderr, "Calling equal_set over (%s, %d) & (%s, %d)\n", dest->id, d, src->id, s);

   switch(d)
   {
      case IN_SET:
	 dlp = dest->defin.inl;
	 dp = dest->defin.inp;
	 di = &dest->defin.ini;
	 break;

      case OUT_SET:
	 dlp = dest->defin.outl;
	 dp = dest->defin.outp;
	 di = &dest->defin.outi;
	 break;

      case GEN_SET:
	 dlp = dest->defin.genl;
	 dp = dest->defin.genp;
	 di = &dest->defin.geni;
	 break;

      case KILL_SET:
	 dlp = dest->defin.killl;
	 dp = dest->defin.killp;
	 di = &dest->defin.killi;
	 break;

      default:
	 printf("Unknows dest type[%d]\n", d);
	 return;
   }

   switch(s)
   {
      case IN_SET:
	 slp = src->defin.inl;
	 sp = src->defin.inp;
	 si = &src->defin.ini;
	 break;

      case OUT_SET:
	 slp = src->defin.outl;
	 sp = src->defin.outp;
	 si = &src->defin.outi;
	 break;

      case GEN_SET:
	 slp = src->defin.genl;
	 sp = src->defin.genp;
	 si = &src->defin.geni;
	 break;

      case KILL_SET:
	 slp = src->defin.killl;
	 sp = src->defin.killp;
	 si = &src->defin.killi;
	 break;

      default:
	 printf("Unknows src type[%d]\n", s);
	 return;
   }
   
   if( *di != *si )
     return 0;

   for(i = 0; i < *si; i++) {
     isthere = 0;
     for(j = 0; j < *di; j++) 
       if(dlp[i] == slp[j] && dp[i] == sp[j]) {
	 isthere = 1;
	 break;
       }
     if(!isthere)
       return 0;
   }
   return 1;
}

void copy_set(struct block *dest, int d, struct block *src, int s) {
   int *dlp;
   struct defuse **dp;
   int *di;

   int *slp;
   struct defuse **sp;
   int *si;

   int i, j;
   int found;

   switch(d)
   {
      case IN_SET:
	 dlp = dest->defin.inl;
	 dp = dest->defin.inp;
	 di = &dest->defin.ini;
	 break;

      case OUT_SET:
	 dlp = dest->defin.outl;
	 dp = dest->defin.outp;
	 di = &dest->defin.outi;
	 break;

      case GEN_SET:
	 dlp = dest->defin.genl;
	 dp = dest->defin.genp;
	 di = &dest->defin.geni;
	 break;

      case KILL_SET:
	 dlp = dest->defin.killl;
	 dp = dest->defin.killp;
	 di = &dest->defin.killi;
	 break;

      default:
	 printf("Unknows dest type[%d]\n", d);
	 return;
   }

   switch(s)
   {
      case IN_SET:
	 slp = src->defin.inl;
	 sp = src->defin.inp;
	 si = &src->defin.ini;
	 break;

      case OUT_SET:
	 slp = src->defin.outl;
	 sp = src->defin.outp;
	 si = &src->defin.outi;
	 break;

      case GEN_SET:
	 slp = src->defin.genl;
	 sp = src->defin.genp;
	 si = &src->defin.geni;
	 break;

      case KILL_SET:
	 slp = src->defin.killl;
	 sp = src->defin.killp;
	 si = &src->defin.killi;
	 break;

      default:
	 printf("Unknows src type[%d]\n", s);
	 return;
   }
   
   *di = *si;
   for(i = 0; i < *si; i++) {
     dlp[i] = slp[i];
     dp[i] = sp[i];
   }
   
}

void union_set(struct block *dest, int d, struct block *src, int s)
{
   int *dlp;
   struct defuse **dp;
   int *di;

   int *slp;
   struct defuse **sp;
   int *si;

   int i, j;
   int found;

   switch(d)
   {
      case IN_SET:
	 dlp = dest->defin.inl;
	 dp = dest->defin.inp;
	 di = &dest->defin.ini;
	 break;

      case OUT_SET:
	 dlp = dest->defin.outl;
	 dp = dest->defin.outp;
	 di = &dest->defin.outi;
	 break;

      case GEN_SET:
	 dlp = dest->defin.genl;
	 dp = dest->defin.genp;
	 di = &dest->defin.geni;
	 break;

      case KILL_SET:
	 dlp = dest->defin.killl;
	 dp = dest->defin.killp;
	 di = &dest->defin.killi;
	 break;

      default:
	 printf("Unknows dest type[%d]\n", d);
	 return;
   }

   switch(s)
   {
      case IN_SET:
	 slp = src->defin.inl;
	 sp = src->defin.inp;
	 si = &src->defin.ini;
	 break;

      case OUT_SET:
	 slp = src->defin.outl;
	 sp = src->defin.outp;
	 si = &src->defin.outi;
	 break;

      case GEN_SET:
	 slp = src->defin.genl;
	 sp = src->defin.genp;
	 si = &src->defin.geni;
	 break;

      case KILL_SET:
	 slp = src->defin.killl;
	 sp = src->defin.killp;
	 si = &src->defin.killi;
	 break;

      default:
	 printf("Unknows src type[%d]\n", s);
	 return;
   }

   for(i = 0; i < *si; i++)
   {
      found = 0;

      for(j = 0; j < *di; j++)
	 if(dlp[j] == slp[i])
	    found = 1;

      if(!found)
      {
	 dlp[*di] = slp[i];
	 dp[*di] = sp[i];
	 (*di)++;
      }
   }
}

void sub_set(struct block *dest, int d, struct block *src, int s)
{
   int *dlp;
   struct defuse **dp;
   int *di;

   int *slp;
   int *si;

   int i, j, k;

   switch(d)
   {
      case IN_SET:
	 dlp = dest->defin.inl;
	 dp = dest->defin.inp;
	 di = &dest->defin.ini;
	 break;

      case OUT_SET:
	 dlp = dest->defin.outl;
	 dp = dest->defin.outp;	
	 di = &dest->defin.outi;
	 break;

      case GEN_SET:
	 dlp = dest->defin.genl;
	 dp = dest->defin.genp;	
	 di = &dest->defin.geni;
	 break;

      case KILL_SET:
	 dlp = dest->defin.killl;
	 dp = dest->defin.killp;	
	 di = &dest->defin.killi;
	 break;

      default:
	 printf("Unknows dest type[%d]\n", d);
	 return;
   }

   switch(s)
   {
      case IN_SET:
	 slp = src->defin.inl;
	 si = &src->defin.ini;
	 break;

      case OUT_SET:
	 slp = src->defin.outl;
	 si = &src->defin.outi;
	 break;

      case GEN_SET:
	 slp = src->defin.genl;
	 si = &src->defin.geni;
	 break;

      case KILL_SET:
	 slp = src->defin.killl;
	 si = &src->defin.killi;
	 break;

      default:
	 printf("Unknows src type[%d]\n", s);
	 return;
   }

   for(i = 0; i < *si; i++)
      for(j = 0; j < *di; j++)
	 if(dlp[j] == slp[i]) 
	 {
	    for(k = j; k < *di - 1; k++) 
	    {
	       dlp[k] = dlp[k+1];		    
	       dp[k] = dp[k+1];
	    }
	    (*di)--;
	 }
}


