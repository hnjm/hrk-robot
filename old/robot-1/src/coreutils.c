#include "coreutils.h"
#ifdef HAS_ARDUINO
#  include <Arduino.h>
#  include <stdio.h>
#  include <stdint.h>
#  include <util/delay.h>
#endif
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <util/delay.h>
#include "coreutils.h"
#include "uart.h"
#include "robot.h"

static const struct robot_s coreutils_programs[] = {
  {"echo" ,"<text...>" ,coreutils_echo},
  {"read" ,"[@<uart>]" ,coreutils_gets},
  {"info" ,""          ,coreutils_info},
  {"kill" ,"NAMES..."  ,coreutils_kill},
# ifdef _UART_H_
  {"pipe" ,"[@<uart>][...][h|s][@<uart>][...]",coreutils_pipe},
  {"start","[@<uart>][...]: Add \"START\\r\\n\" start flag.",coreutils_start},
# endif
# ifdef HAS_ARDUINO
  {"sleep","MS"        ,coreutils_delay},
  {"blink","PINS..."   ,coreutils_blink},
  {"on"   ,"PINS..."   ,coreutils_pin_on},
  {"off"  ,"PINS..."   ,coreutils_pin_off},
# endif
  {NULL,NULL,NULL}
};
void coreutils_init(void) {
  add_commands(coreutils_programs);
}

#ifdef HAS_ARDUINO
int coreutils_blink(int argc,char *argv[]) {
  int i,j,a,b;
  for(i=1;i<argc;i++) {
    int pin = atoi(argv[i]);
    printf("Pin %i" NL,pin);
    pinMode(pin,OUTPUT);
    for(j=0;j<i;j++) {
      digitalWrite(pin,HIGH);
      _delay_ms(500);
      digitalWrite(pin,LOW);
      _delay_ms(500);
    }
    _delay_ms(1000);
  }
  return 0;
}
int coreutils_pin_on(int argc,char *argv[]) {
  int i;
  for(i=1;i<argc;i++) {
    int n = atoi(argv[i]);
    pinMode(n,OUTPUT);
    digitalWrite(n,HIGH);
  }
}
int coreutils_pin_off(int argc,char *argv[]) {
  int i;
  for(i=1;i<argc;i++) {
    int n = atoi(argv[i]);
    pinMode(n,OUTPUT);
    digitalWrite(n,LOW);
  }
}
#endif


int coreutils_echo(int argc,char *argv[]) {
  int i,j; char *k;
  for(i=1;i<argc;i++) {
    if(i!=1) fputs(" ",stdout);
    for(k=argv[i];*k;k++) fputc(*k,stdout);
  }
  fputc('\r',stdout);
  fputc('\n',stdout);
}
int coreutils_gets(int argc,char *argv[]) {
  char buffer[64];
  FILE *fp;
  fp = (FILE*) get_object(argv[1],stdin);
  if(fgets(buffer,sizeof(buffer),fp)) {
    fputs(buffer,stdout);
  }
}
int coreutils_info(int argc,char *argv[]) {
  struct {
    const char *name;
    int   len;
  } types [] = {
    {"char" ,sizeof(char)},
    {"int"  ,sizeof(int)},
    {"long" ,sizeof(long)},
    {"long long",sizeof(long long)},
    {NULL,0}
  };
  int i;
  for(i=0;types[i].name;i++) {
    printf("%-10s = %i Bytes" NL,types[i].name,types[i].len);
  }
  return 0;
}
int coreutils_delay(int argc,char *argv[]) {
  if(argc>1) {
    int i,j;
    for(i=0,j=atoi(argv[1]);i<j;i++) {
      _delay_ms(1);
    }
  }
}
int coreutils_kill (int argc,char *argv[]) {
  int i;
  for(i=1;i<argc;i++) {
    del_hook(argv[i]);
  }
}
#ifdef _UART_H_
int coreutils_pipe (int argc,char *argv[]) {
  int i; FILE_UART *fp;
  del_hook("pipe");
  int (*hook) (void *) = ( int (*) (void*)) pipe;
  for(i=1;i<argc;i++) {
    if(!strcasecmp(argv[i],"h")) {
      hook = ( int (*) (void*)) hexpipe;
    } else {
      fp = (FILE_UART*) get_object(argv[i],NULL);
      if(!fp) {
	fprintf(stderr,"ERROR: Object '%s' not found." NL,argv[i]);
      }
      add_hook(hook,fp,"pipe");
    }
  }
}
int coreutils_start(int argc,char *argv[]) {
  int i; FILE_UART *fp;
  for(i=1;i<argc;i++) {
    fp = (FILE_UART*) get_object(argv[i],NULL);
    if(!fp) {
      fprintf(stderr,"ERROR: Object '%s' not found." NL,argv[i]);
    }
    fp->initflag = "START\r\n";
    fp->ptr = NULL;
  }
}
#endif
