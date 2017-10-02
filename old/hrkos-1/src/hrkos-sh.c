/**
**********************************************************************************
* @brief Shell hook for hrkos.
**********************************************************************************/
#include "hrkos.h"
#ifndef HRKOS_SH_MAX_PROGRAMS
#  define HRKOS_SH_MAX_PROGRAMS 20
#endif
#include <string.h>

/* Some debugging programs. */
int echo_code(FILE *fp,int argc,char **argv) {
     int i;
     if(argc>=1) fputs(argv[1],fp);
     for(i=2;i<argc;i++) {
	  fputc(' ',fp);
	  fputs(argv[i],fp);
     }
     fputs(NL,fp);
     return 0;
}
int comment_code(FILE *fp,int argc,char **argv) {
     return 0;
}
int help_code(FILE *fp,int argc,char **argv);

/* GLOBALS. */
static FILE *sfp = NULL;
static struct {
     char *name;
     char *hlp;
     int (*code) (FILE *fp,int argc,char **argv);
} programs[HRKOS_SH_MAX_PROGRAMS+1] = {
     {"echo","Repeat said.",echo_code},
     {"#"   ,"Comment"     ,comment_code},
     {"help","Print help"  ,help_code},
     {NULL,NULL,NULL}
};

int help_code(FILE *fp,int argc,char **argv) {
     int i;
     for(i=0;i<HRKOS_SH_MAX_PROGRAMS;i++) {
	  if(!programs[i].name) break;
	  fprintf(fp,YELLOW("%-20s") " : %s" NL,programs[i].name,programs[i].hlp);
     }
}




/* Define new programs with this. */
void program(char *name,char *hlp,int (*code) (FILE *fp,int argc,char **argv)) {
     int i=0;
     for(i=0;i<HRKOS_SH_MAX_PROGRAMS;i++) {
	  if(!programs[i].name) {
	       programs[i].name = name;
	       programs[i].hlp  = hlp;
	       programs[i].code = code;
	       programs[i+1].name = NULL;
	       break;
	  }
     }
}

/* Parse line and execute. */
int system(const char *command) {
     char *p,*a[20]; int i,j;
     char buffer[64] = { 0 };
     strncpy(buffer,command,63);
     /* Split string. */
     for(i=0,a[i]=strtok_r(buffer," \t\n",&p);
	 a[i] && i<19;
	 i++,a[i]=strtok_r(NULL," \t\n",&p)) {}
     /* Empty line. */
     if(!a[0]) return 0;
     /* Search command. */
     for(j=0;j<HRKOS_SH_MAX_PROGRAMS;j++) {
	  if(programs[j].name == NULL) break;
	  if(!strcasecmp(a[0],programs[j].name)) {
	       return programs[j].code(sfp,i,a);
	  }
     }
     /* Write. */
     fprintf(sfp,RED("ERR: ") "Command `%s` not found. (Type `help` for a list)" NL,a[0]);
     return 1;
}




static void line_handler (unsigned char *line) {
     if(system(line)==0) {
	  fputs(GREEN("> "),sfp);
     } else {
	  fputs(RED("> "),sfp);
     }
}
static void control_handler(char ctrl) {
     char *prog = NULL;
     switch(ctrl) {
     case 'A': prog = UP_PROGRAM;    break;
     case 'B': prog = DOWN_PROGRAM;  break;
     case 'C': prog = RIGHT_PROGRAM; break;
     case 'D': prog = LEFT_PROGRAM;  break;
     }
     if(prog) {
	  fputs(prog,sfp);
	  fputs(NL,sfp);
	  line_handler(prog);
     }
}
void hrkos_hook_shell(FILE *fp) {
     if(!sfp) {
	  sfp = fp;
	  hrkos_getlines(fp,line_handler,control_handler);
     }
}



