#ifndef _UART_H_
#define _UART_H_
#include <stdio.h>
#define NL "\r\n"
typedef struct {
  FILE  fp;       /* udata=(void*)uart */
  char  newline;  /* 0    : Do not use. */
  const char *initflag; /* NULL : uart_start FLAG. */
  const char *ptr;      /* NULL : Do not use*/
} FILE_UART;
void uart_init          (FILE_UART *self  ,int uart,unsigned long baud,const char *start_flag);
void uart_setbaud       (FILE_UART *self,unsigned long baud);
int  uart_getchar_nolock(FILE_UART *stream,char *out);
int  uart_start         (FILE_UART *stream);
int  pipe               (FILE_UART *from);
int  hexpipe            (FILE_UART *from);
#define GET_UBRR_VALUE(baud) (((F_CPU) + 8UL * (baud)) / (16UL * (baud)) -1UL)
#define GET_UBRRL_VALUE(baud) (GET_UBRR_VALUE(baud) & 0xff)
#define GET_UBRRH_VALUE(baud) (GET_UBRR_VALUE(baud) >> 8)





#endif
