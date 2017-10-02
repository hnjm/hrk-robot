#ifndef _HRK_UART_C_
#include <stdio.h>
#ifndef HRK_FIFO_LENGTH
#  define HRK_FIFO_LENGTH 64
#endif
typedef struct hrk_fifo_t {
     int  i,o;
     char buffer[HRK_FIFO_LENGTH];
     int  uart;
} hrk_fifo_t;
char  hrk_fifo_get(volatile hrk_fifo_t *fifo);
void  hrk_fifo_put(volatile hrk_fifo_t *fifo,char data);
FILE *hrk_uart_open(int uart,int baudrate); /* The first to open becomes stdin/stdout/stderr. */
void  hrk_uart_setbaud(int uart,unsigned long baud);
int   hrk_uart_putchar(char c,FILE *stream);
#endif

#include <avr/io.h>
#include <stdio.h>
#include <util/setbaud.h>
#include <string.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

static FILE       fp[3];
static hrk_fifo_t fp_fifo[3];

#define GET_UBRR_VALUE(baud) (((F_CPU) + 8UL * (baud)) / (16UL * (baud)) -1UL)
#define GET_UBRRL_VALUE(baud) (GET_UBRR_VALUE(baud) & 0xff)
#define GET_UBRRH_VALUE(baud) (GET_UBRR_VALUE(baud) >> 8)


ISR(USART0_RX_vect) {
     hrk_fifo_put(&fp_fifo[0],UDR0);
}
#ifdef UDR1
ISR(USART1_RX_vect) {
     hrk_fifo_put(&fp_fifo[1],UDR1);
}
#endif


/* FIFO */
char  hrk_fifo_get(volatile hrk_fifo_t *fifo) {
     char c;
     while(fifo->i == fifo->o) {
	  wdt_reset();
     }
     cli();
     c = fifo->buffer[fifo->o];
     fifo->o = (fifo->o + 1) % HRK_FIFO_LENGTH;
     sei();
     return c;
}
void  hrk_fifo_put(volatile hrk_fifo_t *fifo,char data) {
     cli();
     fifo->buffer[fifo->i] = data;
     fifo->i = (fifo->i + 1) % HRK_FIFO_LENGTH;
     if(fifo->i == fifo->o) fifo->i--; /* Buffer overflow. */
     sei();
}






/* Blocking output. */
int hrk_uart_putchar(char c,FILE *stream) {
     int uart = ((hrk_fifo_t*)stream->udata)->uart;
     switch (uart) {
     case 0:
	  loop_until_bit_is_set(UCSR0A,UDRE0);
	  UDR0 = c;
	  break;
#    ifdef UDR1
     case 1:
	  loop_until_bit_is_set(UCSR1A,UDRE1);
	  UDR1 = c;
	  break;
#    endif
     }
     return c;
}










/* Blocking input. */
int hrk_uart_getchar_o(FILE *stream) {
     char c;
     int uart = ((hrk_fifo_t*)stream->udata)->uart;
     switch (uart) {
     case 0:
	  loop_until_bit_is_set(UCSR0A,RXC0);
	  c = UDR0;
	  return c;
#    ifdef UDR1
     case 1:
	  loop_until_bit_is_set(UCSR1A,RXC1);
	  c = UDR1;
	  return UDR1;
#    endif
  }
}
int hrk_uart_getchar(FILE *stream) {
     int  uart = ((hrk_fifo_t*)stream->udata)->uart;
     char c    = hrk_fifo_get(&fp_fifo[uart]);
     return c;
}
/* No locking input. */
int hrk_uart_getchar_nolock(FILE *stream,char *out) {
     int uart = ((hrk_fifo_t*)stream->udata)->uart;
     switch (uart) {
     case 0:
	  if(!(UCSR0A & _BV(RXC0))) return 0;
	  *out = UDR0;
	  return 1;
#    ifdef UDR1
     case 1:
	  if(!(UCSR1A & _BV(RXC1))) return 0;
	  *out = UDR1;
	  return 1;
#    endif
     }
     return 0;
}










/* Set baudrate. */
void hrk_uart_setbaud(int uart,unsigned long baud) {
     switch(uart) {
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







FILE *hrk_uart_open(int uart,int baudrate) {
     static int uart_initialized = 0;
     cli();
     fp_fifo[uart].uart = uart;
     fp_fifo[uart].i    = 0;
     fp_fifo[uart].o    = 0;
     fp[uart].udata = &fp_fifo[uart];
     fp[uart].put   = hrk_uart_putchar;
     fp[uart].get   = hrk_uart_getchar;
     fp[uart].flags = _FDEV_SETUP_READ|_FDEV_SETUP_WRITE;
     hrk_uart_setbaud(uart,baudrate);
     /* 8bit, no parity, tx/rx enable. */
     switch (uart) {
     case 0:
	  UCSR0C = _BV(UCSZ01)| _BV(UCSZ00);
	  UCSR0B = _BV(RXEN0) | _BV(TXEN0) | (1 << RXCIE0);
	  break;
# ifdef UDR1
     case 1:
	  UCSR1C = _BV(UCSZ11)| _BV(UCSZ10);
	  UCSR1B = _BV(RXEN1) | _BV(TXEN1) | (1 << RXCIE1);
	  break;
# endif
     default:
	  break;
     }
     if(!uart_initialized) {
	  stdin  = &fp[uart];
	  stdout = &fp[uart];
	  stderr = &fp[uart];
     }
     sei();
     return &fp[uart];
}


