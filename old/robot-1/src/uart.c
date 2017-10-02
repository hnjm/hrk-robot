/**
 * This file configures an UART device.
 */
#ifndef F_CPU
#  error "Please define F_CPU."
#endif
#ifdef BAUD
#  undef BAUD
#  define BAUD 9600
#endif
#ifndef BAUD
#  error "Please define BAUD."
#endif
#include <avr/io.h>
#include <stdio.h>
#include <util/setbaud.h>
#include <string.h>
#include "uart.h"



/* Blocking output. */
int uart_write(char c,int num) {
  switch (num) {
  case 0:
    loop_until_bit_is_set(UCSR0A,UDRE0);
    UDR0 = c;
    break;
# ifdef UDR1
  case 1:
    loop_until_bit_is_set(UCSR1A,UDRE1);
    UDR1 = c;
    break;
# endif
  }
  return c;
}
int uart_putchar(char c,FILE_UART *stream) {
  return uart_write(c,(int)((FILE*)stream)->udata);
}







/* Blocking input. */
int uart_read(int num) {
  switch (num) {
  case 0:
    loop_until_bit_is_set(UCSR0A,RXC0);
    return UDR0;
# ifdef UDR1
  case 1:
    loop_until_bit_is_set(UCSR1A,RXC1);
    return UDR1;
# endif
  }
}
int uart_getchar(FILE_UART *stream) {
  char c;
  /* Transform CRLF and CR to LF. */
  c = uart_read((int)stream->fp.udata);
  if(stream->newline) {
    stream->newline = 0;
    if(c=='\n') {
      c = uart_read((int)stream->fp.udata);
    }
  }
  if(c=='\r') {
    c = '\n';
    stream->newline = 1;
  }
  return c;
}






/* Non blocking input. */
int uart_read_nolock(int num,char *out) {
  switch (num) {
  case 0:
    if(!(UCSR0A & _BV(RXC0))) return 0;
    *out = UDR0;
    return 1;
# ifdef UDR1
  case 1:
    if(!(UCSR1A & _BV(RXC1))) return 0;
    *out = UDR1;
    return 1;
# endif
  }
  return 0;
}
int uart_start(FILE_UART *stream) {
  char c;
  if(!stream->initflag) return 1; /* Already flagged, or not needed. */
  if(!uart_read_nolock((int)stream->fp.udata,&c)) {
    return 0;
  }
  if(!stream->ptr) stream->ptr = stream->initflag;
  if(c==*stream->ptr) {
    stream->ptr++;
  } else {
    stream->ptr = stream->initflag;
  }
  if(*stream->ptr=='\0') {
    stream->initflag = NULL;
    return 1;
  }
  return 0;
}
int uart_getchar_nolock(FILE_UART *stream,char *out) {
  if(uart_start(stream)) {
    return uart_read_nolock((int)stream->fp.udata,out);
  }
  return 0;
}



int pipe(FILE_UART *from) {
  char c;
  if(!uart_getchar_nolock(from,&c)) return 0;
  putc(c,stdout);
  return 0;
}
int hexpipe(FILE_UART *from) {
  char c;
  if(!uart_getchar_nolock(from,&c)) return 0;
  printf(":%02X",c);
  return 0;
}













void uart_setbaud(FILE_UART *self,unsigned long baud) {
  switch((int)self->fp.udata) {
  case 0:
    UBRR0H = GET_UBRRH_VALUE(baud);;
    UBRR0L = GET_UBRRL_VALUE(baud);;
    break;
# ifdef UDR1
  case 1:
    UBRR1H = GET_UBRRH_VALUE(baud);
    UBRR1L = GET_UBRRL_VALUE(baud);
    break;
# endif
  }
}
void uart_init(FILE_UART *self,int uart,unsigned long baud,const char *start_flag) {
  self->fp.udata = (void*) uart;
  self->fp.put    = (int (*) (char,FILE*)) uart_putchar;
  self->fp.get    = (int (*) (FILE*))      uart_getchar;
  self->fp.flags  = _FDEV_SETUP_READ|_FDEV_SETUP_WRITE;
  self->newline   = 0;
  self->initflag  = start_flag;
  self->ptr       = NULL;
  uart_setbaud(self,baud);
  /* 8bit, no parity, tx/rx enable. */
  switch (uart) {
  case 0:
    UCSR0C = _BV(UCSZ01)| _BV(UCSZ00);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    break;
# ifdef UDR1
  case 1:
    UCSR1C = _BV(UCSZ11)| _BV(UCSZ10);
    UCSR1B = _BV(RXEN1) | _BV(TXEN1);
    break;
# endif
  default:
    break;
  }
}

