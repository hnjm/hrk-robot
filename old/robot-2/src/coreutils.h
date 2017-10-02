#ifndef _HRK_COREUTILS_H_
#define _HRK_COREUTILS_H_
#include "config.h"
#  ifdef HAS_ARDUINO
int coreutils_blink   (int argc,char *argv[]);
int coreutils_delay   (int argc,char *argv[]);
int coreutils_pin_on  (int argc,char *argv[]);
int coreutils_pin_off (int argc,char *argv[]);
#  endif
int coreutils_echo  (int argc,char *argv[]);
int coreutils_help  (int argc,char *argv[]);
int coreutils_info  (int argc,char *argv[]);
int coreutils_delay (int argc,char *argv[]);
int coreutils_set   (int argc,char *argv[]);
int coreutils_gets  (int argc,char *argv[]);
int coreutils_kill  (int argc,char *argv[]);
int coreutils_pipe  (int argc,char *argv[]);
int coreutils_start (int argc,char *argv[]);

void coreutils_init(void);
#endif

