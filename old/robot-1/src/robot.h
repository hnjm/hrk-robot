#ifndef _ROBOT_H_
#define _ROBOT_H_
#include "uart.h"
struct robot_s {
  const char *name;
  const char *help;
  int (*program) (int argc,char *argv[]);
};
typedef struct {
  FILE_UART *fp;
  int flags;
# define TTY_USE_PROMPT   0x01
# define TTY_USE_NOECHO   0x02
# define TTY_STATE_INIT   0x04
  int point;
  char buffer[64];
} tty_t;
/* Shell. */
int  robot(const char *command);
int  tty  (tty_t *info);
/* Shell control. */
void add_command(const char *name,
		 int (*cmd) (int argc,char *argv[]),
		 const char *help);
void  add_object (const char *name,void *object);
void  del_object (const char *name);
void *get_object (const char *name,void *def);
void add_hook   (int (*ptr) (void *arg),void *arg,const char *name);
void del_hook   (const char *name);
int  chk_hook   (const char *name);

void add_commands(const struct robot_s *cmds);
/* Main hook. */
void robot_hook(void);
/* Main inits. */
void robot_init(FILE_UART *fp);
void tty_add(tty_t *i,FILE_UART *fp,const char *hook_name);
#endif
