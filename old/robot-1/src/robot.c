/**
************************************************************************
*
*
************************************************************************/
#include <string.h>
#include <avr/wdt.h>
#include <setjmp.h>
#include <avr/interrupt.h>
#include "robot.h"
#include "uart.h"
#ifndef ROBOT_MAX_OBJECTS
#  define ROBOT_MAX_OBJECTS 20
#endif
#ifndef ROBOT_MAX_HOOKS
#  define ROBOT_MAX_HOOKS 6
#endif
int robot_set(int argc,char *argv[]);
int robot_help(int argc,char *argv[]);
int robot_ps(int argc,char *argv[]);
int robot_objs(int argc,char *argv[]);
static struct robot_s programs[20] = {
  {"set"  ,"n (no echo) N (echo) p (no prompt) P (prompt)",robot_set  },
  {"help" ,"Print help about commands."                   ,robot_help },
  {"ps"   ,""                                             ,robot_ps   },
  {"objs" ,""                                             ,robot_objs },
  {NULL   ,NULL                                           ,NULL       }
};
static struct {
  const char *name;
  void       *obj;
} objects[ROBOT_MAX_OBJECTS];
static struct {
  int (*ptr) (void *arg);
  void *arg;
  const char *name;
} hooks[ROBOT_MAX_HOOKS];
static jmp_buf stack;
static int robot_flags = 0;
#define ROBOT_STARTED_FLAG 0x01
ISR(WDT_vect) {
  longjmp(stack,1);
}

void tty_add(tty_t *i,FILE_UART *fp,const char *hook_name) {
  memset(i,0,sizeof(tty_t));
  i->fp     = fp;
  i->flags |= TTY_USE_PROMPT;
  add_hook((int (*) (void*)) tty,i,hook_name);
  add_object(hook_name,fp);
}
void robot_init(FILE_UART *fp) {
  int i;
  if(!(robot_flags&ROBOT_STARTED_FLAG)) {
    for(i=0;i<ROBOT_MAX_HOOKS;i++) {
      hooks[i].name = NULL;
    }
    for(i=0;i<ROBOT_MAX_OBJECTS;i++) {
      objects[i].name = NULL;
    }
    robot_flags |= ROBOT_STARTED_FLAG;
  }
  stdout = (FILE*)fp;
  stdin  = (FILE*)fp;
  stderr = (FILE*)fp;
}
  
/* Evaluate commands. */
static tty_t *current_info;
int robot(const char *cmd) {
  char *args[20],*j,*jj,*command;
  /* Split argument. */
  for(command=strtok_r((char*)cmd,";",&jj);
      command;
      command=strtok_r(NULL,";",&jj)) {
    int i,argc;
    for(argc=0,args[0]=strtok_r((char*)command," \r\t\n",&j);
	args[argc]&&argc<19;
	args[++argc]=strtok_r(NULL," \t\n\r",&j)) {
    }
    /* Skip errors. */
    if(i==19) {
      fprintf(stderr,"ERROR: Too much arguments." NL);
      return 1;
    }
    if(!args[0]) {
      return 1;
    }
    /* Search command. */
    for(i=0;programs[i].name;i++) {
      if(!strcmp(programs[i].name,args[0])) break;
    }
    if(!programs[i].name) {
      fprintf(stderr,"ERROR: Command `%s` not found." NL,args[0]);
      return 1;
    }
    /* Perform redirection. */
    FILE *orig_stdin  = stdin;
    FILE *orig_stdout = stdout;
    for(;argc>1;argc--,args[argc] = NULL) {
      char *k = args[argc-1];
      if(!(*k=='>'||*k=='<')) break;
      FILE *fp = (FILE*) get_object(k+1,NULL);
      if(!fp) {
	fprintf(stderr,"ERROR: Object '%s' not found." NL,k+1);
	return 1;
      }
      switch(*k) {
      case '>': stdout = fp; break;
      case '<': stdin  = fp; break;
      }
    }
    /* Run command. */
    programs[i].program(argc,args);
    /* Restore from redirection. */
    stdin  = orig_stdin;
    stdout = orig_stdout;
  }
}
int tty(tty_t *info) {
  char c;
  /* Set default values. */
  if(!(info->flags&TTY_STATE_INIT)) {
    info->flags |= TTY_STATE_INIT;
    if(!info->fp) {
      info->fp     = (FILE_UART*) stdout;
      info->flags |= TTY_USE_PROMPT;
    }
    info->point = 0;
  }
  /* Check input. */
  if(!uart_getchar_nolock(info->fp,&c))
    return 0;
  /* Backspace */
  if(c==0x8) {
    if (info->point!=0) {
      char b[] = {0x8,' ',0x8,0};
      fputs(b,(FILE*) info->fp);
      info->point--;
    }
    return 0;
  }
  
  /* Detect end of line. */
  if(c=='\r') {
    info->fp->newline = 1;
  }
  if(c=='\r'||c=='\n') c='\0';
  /* Trash characters. */
  if(info->point==0&&c=='\0') return 0;
  /* Echo character. */
  if(!(info->flags&TTY_USE_NOECHO))
    putc((c)?c:'\n',(FILE*)info->fp);
  /* Save character. */
  info->buffer[info->point] = c;
  info->point++;
  /* If newline, execute. */
  if(c=='\0') {
    FILE *f0,*f1,*f2;
    /* Store default stdout,stdin,stderr. */
    current_info = info;
    f0 = stdin;        f1 = stdout;       f2 = stderr;
    stdout = (FILE*) info->fp; stderr = (FILE*) info->fp; stdin  = (FILE*) info->fp;
    /* Run command. */
    robot(info->buffer);
    current_info = NULL;
    /* Restore defaults. */
    stdout = f1; stderr = f1; stdin  = f0;
    info->point  = 0;
    /* Print prompt. */
    if(info->flags&TTY_USE_PROMPT) {
      fputs("> ",(FILE*)info->fp);
    }
  }
  return 0;
}
void add_command(const char *name,int (*cmd) (int argc,char *argv[]),const char *help) {
  int i;
  for(i=0;programs[i].name;i++) { }
  programs[i].name    = name;
  programs[i].help    = help;
  programs[i].program = cmd;
  i++;
  programs[i].name    = NULL;
  programs[i].help    = NULL;
  programs[i].program = NULL;
}
void add_commands(const struct robot_s *cmds) {
  int i,j;
  for(i=0;programs[i].name;i++) { }
  for(j=0;cmds[j].name;j++,i++) {
    programs[i].name    = cmds[j].name;
    programs[i].help    = cmds[j].help;
    programs[i].program = cmds[j].program;
  }
  programs[i].name    = NULL;
  programs[i].help    = NULL;
  programs[i].program = NULL;
}
void add_object (const char *name,void *object) {
  int i;
  del_object(name);
  for(i=0;objects[i].name;i++) {}
  objects[i].name = name;
  objects[i].obj = object;
}
void del_object (const char *name) {
  int i;
  for(i=0;i<ROBOT_MAX_OBJECTS;i++) {
    if(!strcasecmp(name,objects[i].name)) {
      objects[i].name = NULL;
    }
  }
}
void *get_object (const char *name,void *def) {
  int i;
  if(!name) return def;
  for(i=0;i<ROBOT_MAX_OBJECTS;i++) {
    if(!strcasecmp(name,objects[i].name)) {
      return objects[i].obj;
    }
  }
  return def;
}

void add_hook   (int (*ptr) (void *arg),void *arg,const char *name) {
  int i;
  for(i=0;hooks[i].name;i++) {}
  hooks[i].ptr  = ptr;
  hooks[i].arg  = arg;
  hooks[i].name = name;
}
void del_hook   (const char *name) {
  int i;
  for(i=0;i<ROBOT_MAX_HOOKS;i++) {
    if(!hooks[i].ptr)  continue;
    if(!hooks[i].name) continue;
    if(!strcasecmp(hooks[i].name,name)) {
      hooks[i].name = NULL;
    }
  }
}
int chk_hook (const char *name) {
  int i,j;
  for(i=0,j=0;i<ROBOT_MAX_HOOKS;i++) {
    if(!hooks[i].ptr)  continue;
    if(!hooks[i].name) continue;
    if(!strcasecmp(hooks[i].name,name)) {
      j++;
    }
  }
  return j;
}


void robot_hook(void) {
  int i;
  FILE *fp1 = stdout;
  FILE *fp2 = stderr;
  FILE *fp0 = stdin;
  sei();
  for(i=0;i<ROBOT_MAX_HOOKS;i++) {
    if(hooks[i].name) {
      if(setjmp(stack)) {
	fprintf(fp2,"WDT: Removed hook '%s' ." NL,hooks[i].name);
	stdout = fp1;
	stderr = fp2;
	stdin  = fp0;
	hooks[i].name = NULL;
      } else {
	wdt_enable(WDTO_8S);
	WDTCSR |= _BV(WDIE);
	WDTCSR &= ~(_BV(WDE));
	if(hooks[i].ptr(hooks[i].arg)) {
	  hooks[i].name = NULL;
	}
      }
      wdt_disable();
    }
  }
  
}
void robot_tty_init(FILE_UART *out_uart,tty_t *out_info,int uart,unsigned long baud) {
  memset(out_info,0,sizeof(tty_t));
  uart_init(out_uart,uart,baud,NULL);
  out_info->fp     = out_uart;
  out_info->flags |= TTY_USE_PROMPT;
  add_hook((int (*) (void*)) tty,out_info,"main");
  add_object("@main",out_uart);
  
  
}


/*************************************************************/
int robot_objs(int argc,char *argv[]) {
  int i;
  for(i=0;i<ROBOT_MAX_OBJECTS;i++) {
    if(objects[i].name) {
      printf("%-10s %-8p\n",
	     objects[i].name,
	     (objects[i].obj)?objects[i].obj:"0x0");
    }
  }
}
int robot_ps(int argc,char *argv[]) {
  int i;
  for(i=0;i<ROBOT_MAX_HOOKS;i++) {
    if(hooks[i].name) {
      printf("%-10s %-8p %-8p\n",
	     (hooks[i].name)?hooks[i].name:"none",
	     (hooks[i].ptr)?(void*)hooks[i].ptr:"0x0",
	     (hooks[i].arg)?hooks[i].arg:"0x0");
    }
  }
}
int robot_set(int argc,char *argv[]) {
  int i; char *c;
  for(i=1;i<argc;i++) {
    for(c=&argv[i][0];*c;c++) {
      switch(*c) {
      case 'n':
	current_info->flags |=   TTY_USE_NOECHO;
	break;
      case 'N':
	current_info->flags &= ~(TTY_USE_NOECHO);
	break;
      case 'p':
	current_info->flags &= ~(TTY_USE_PROMPT);
	break;
      case 'P':
	current_info->flags |=   TTY_USE_PROMPT;
	break;
      default:
	fprintf(stderr,"ERROR: '%c' not recognized." NL,c);
      }
    }
  }
}
int robot_help(int argc,char *argv[]) {
  int i,j;
  switch (argc) {
  case 1:
    fputs("Syntax: COMMAND ARGS... [>@FP][<@FP]" NL
	  "Available commands:",stdout);
    for(i=0;programs[i].name;i++) {
      fputs((i%7)?" ":NL,stdout);
      fputs(programs[i].name,stdout);
    }
    fputs(NL,stdout);
  default:
    for(j=1;j<argc;j++) {
      for(i=0;programs[i].name;i++) {
	if(!programs[i].help) continue;
	if(!strcmp(programs[i].name,argv[j])) {
	  fputs(programs[i].name,stdout);
	  fputs(" ",stdout);
	  fputs(programs[i].help,stdout);
	  fputs(NL,stdout);
	}
      }
    }
  }
}
