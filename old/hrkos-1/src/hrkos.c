#include "hrkos.h"
#include <string.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <Arduino.h>

#ifndef HRKOS_COUNTER_COUNT
#  define HRKOS_COUNTER_COUNT 20
#endif

static hrkos_lock_t locks[HRKOS_COUNTER_COUNT];

/**************************************************************************
 * Timer
 * f  = F_CPU/Prescaler
 * sf = f/ovf_value
 * -> ovf_value = F_CPU/Prescaler/sf
 * WGMxx = 010 (Overflow at OCR0A value)
 * CSxx  = 011 (Prescaler to 64)
 * COMxx = 00
 **************************************************************************/
ISR (TIMER0_COMPA_vect) {
     int i;
     for(i=0;i<HRKOS_COUNTER_COUNT;i++) {
	  hrkos_lock_t *l = &locks[i];
	  if(!(l->flags & HRKOS_LOCK_IS_USED)) continue;
	  if( (l->flags & HRKOS_LOCK_TIMED)) {
	       int *sum = (int*) (l->udata);
	       (*sum) += 1;
	  }
     }
}
/*
 * INTERRUPT_TIME=N/(F_CPU/1024)=0.1ms
 * N=10ms*(F_CPU/1024)=F_CPU/102400
 */

#if (F_CPU/64/10000) == 0
#  error Prescaler too big.
#endif
void hrkos_init_timer0() {
     const unsigned char ovf_value = F_CPU/102400;
     OCR0A = ovf_value; /* ovf_value, 10kHz */
     TCCR0A=0b00000010; /* [COM0A{1,0}][COM0B{1,0}][R][R][WGT0{1,0}] */
     TCCR0B=0b00000101; /* [FOC0{A,B}][R][R][WGT02][CS0{2,1,0}] */
     TIMSK0 |= (1 << OCIE0A); //Set the ISR COMPA vect
     sei();
}





/**************************************************************************
 * Lock mechanism, all magick happens here.
 **************************************************************************/
hrkos_lock_t *hrkos_lock_new(int (*check) (void *udata),void *udata,
			     int (*func) (void *udata),int flags) {
     static int started = 0; int i;
     if(started == 0) {
	  hrkos_init_timer0();
	  for(i=0;i<HRKOS_COUNTER_COUNT;i++) locks[i].flags = 0;
	  started = 1;
     }
     /* Get an unused lock. */
     for(i=0;i<20;i++) {
	  if(!(locks[i].flags & HRKOS_LOCK_IS_USED)) {
	       locks[i].flags  = flags;
	       locks[i].flags |= HRKOS_LOCK_IS_USED;
	       /* Set values. */
	       locks[i].check     = check;
	       locks[i].udata     = udata;
	       locks[i].func      = func;
	       /* Return the lock. */
	       return &locks[i];
	  }
     }
     return NULL;
}
void hrkos_burner(void) {
     static int i = 0;
     i = (i+1)%HRKOS_COUNTER_COUNT;
     hrkos_lock_t *l = &locks[i];
     if(!(l->flags & HRKOS_LOCK_IS_USED)) return;
     if( (l->flags & HRKOS_LOCK_IS_LOCKED)) return;
     if(!(l->func)) return;
     /* Perform check. */
     if(l->check) if(!l->check(l->udata)) return;
     /* Perform operation. */
     l->flags |= HRKOS_LOCK_IS_LOCKED;
     if(l->func(l->udata)) {
	  l->flags = 0;
	  if(l->flags & HRKOS_LOCK_FREE) {
	       free(l->udata);
	  }
     } else {
	  l->flags &= ~HRKOS_LOCK_IS_LOCKED;
     }
}
void hrkos_lock_check(hrkos_lock_t *lock) {
     static int inside_check = 0;
     /* Check whether the condition is met. */
     if(lock->check(lock->udata)) { return; }
     lock->flags |= HRKOS_LOCK_IS_LOCKED;
     /* Hang. */
     if(inside_check < 2) { /* fgetc(*), fputc(*), in both smart waiting. */
	  inside_check++;
	  while(!lock->check(lock->udata)) {
	       hrkos_burner();
	  }
	  inside_check--;
     } else {
	  while(!lock->check(lock->udata)) { }
     }
     lock->flags &= ~HRKOS_LOCK_IS_LOCKED;
}
void hrkos_lock_check_lock(hrkos_lock_t *lock) {
     while(lock->check(lock->udata)==0) { }
}






/***********************************************************************
 * I/O
 ***********************************************************************/
#define GET_UBRR_VALUE(baud) (((F_CPU) + 8UL * (baud)) / (16UL * (baud)) -1UL)
#define GET_UBRRL_VALUE(baud) (GET_UBRR_VALUE(baud) & 0xff)
#define GET_UBRRH_VALUE(baud) (GET_UBRR_VALUE(baud) >> 8)

static int hrkos_uart_check_w(hrkos_file_t *uart) {
     int ret = 0;
     switch (uart->num) {
     case 0:	  
	  ret = bit_is_set(UCSR0A,UDRE0);
	  break;
#    ifdef UDR1
     case 1:
	  ret = bit_is_set(UCSR1A,UDRE1);
	  break;
#    endif
     }
     return ret;
}
static int hrkos_uart_putchar(char c,hrkos_file_t *uart) {
     hrkos_lock_check(uart->w_lock);
     switch (uart->num) {
     case 0:
	  UDR0 = c;
	  break;
#         ifdef UDR1
     case 1:
	  UDR1 = c;
	  break;
#         endif
     }
     return c;
}
static int hrkos_uart_check_r(hrkos_file_t *uart) {
     int ret = 0;
     switch (uart->num) {
     case 0:
	  ret = bit_is_set(UCSR0A,RXC0);
	  break;
#    ifdef UDR1
     case 1:
	  ret = bit_is_set(UCSR1A,RXC1);
	  break;
#    endif
     }
     return ret;
}
static int hrkos_uart_getchar(hrkos_file_t *uart) {
     hrkos_lock_check(uart->r_lock);
     char out = -1;
     switch (uart->num) {
     case 0:
	  out = UDR0;
	  break;
#    ifdef UDR1
     case 1:
	  out = UDR1;
	  break;
#    endif
     }
     return out;
}
FILE *hrkos_uart_init(int num,unsigned long baud) {
     hrkos_file_t *uart = (hrkos_file_t *) malloc(sizeof(hrkos_file_t));
     /* Set baudrate. */
     switch(num) {
     case 0:
	  UBRR0H = GET_UBRRH_VALUE(baud);;
	  UBRR0L = GET_UBRRL_VALUE(baud);;
	  break;
#         ifdef UDR1
     case 1:
	  UBRR1H = GET_UBRRH_VALUE(baud);
	  UBRR1L = GET_UBRRL_VALUE(baud);
	  break;
#         endif
     }
     fdev_setup_stream	(&uart->fp,
			 (void*) hrkos_uart_putchar,
			 (void*) hrkos_uart_getchar,
			 _FDEV_SETUP_READ|_FDEV_SETUP_WRITE);
     uart->fp.udata = NULL;
     uart->num = num;
     
     uart->w_lock = hrkos_lock_new((void*)hrkos_uart_check_w,uart,NULL,0);
     uart->r_lock = hrkos_lock_new((void*)hrkos_uart_check_r,uart,NULL,0);
     
     return &uart->fp;
}





/*****************************************************************
 * Hidle operations.
 *****************************************************************/
#define COUNTER_MAX 5
typedef struct {
     int summer;
     int counters[10];
     int scale;
     int value;
} hrkos_timer_t;

/* Summer. */
static int sum_and_compare(hrkos_timer_t *timer) {
     int i,ret = 0;
     static int scales[10] = {100,60,24,7};
     cli();
     timer->counters[0] += timer->summer;
     timer->summer=0;
     sei();
     for(i=1;i<COUNTER_MAX && timer->counters[i-1] >= scales[i-1];i++) {
	  timer->counters[i]++;
	  timer->counters[i-1] = 0;
     }
     if(timer->counters[(timer->scale)] == (timer->value)) ret = 1;
     if(i==COUNTER_MAX || ret) {
          memset(timer->counters,0,sizeof(timer->counters));
     }
     return ret;
}

void hrkos_hook(int scale,int value,int (*fun) (void *ign)) {
     hrkos_timer_t *timer = malloc(sizeof(hrkos_timer_t));
     memset(timer->counters,0,sizeof(timer->counters));
     timer->scale  = scale;
     timer->value  = value;
     timer->summer = 0;
     hrkos_lock_new((void*)sum_and_compare,(void*)timer,fun,HRKOS_LOCK_FREE|HRKOS_LOCK_TIMED);
}
static int hrkos_hook_blink_t(void *ign) {
     static int val = 0;
     pinMode(13,OUTPUT);
     digitalWrite(13,(val)?HIGH:LOW);
     val = !val;
     return 0;
}
void hrkos_hook_blink(int scale,int value) {
     hrkos_hook(scale,value,hrkos_hook_blink_t);
}





/*******************************************************
 * Interactive input.
 *******************************************************/
typedef struct {
     char   buffer[64];
     int    bufpos;
     int    escape;
     void (*line_handler)    (unsigned char *line);
     void (*control_handler) (char ctrl);
} hrkos_getlines_t;
static int hrkos_getlines_f(hrkos_file_t *self) {
     int i;
     hrkos_getlines_t *getlines = self->fp.udata;
     unsigned char car = fgetc((FILE*) self);
     char *buffer =  getlines->buffer;
     int  *bufpos = &getlines->bufpos;
     /* Control characters. */
     if (getlines->escape) {
	  if(getlines->control_handler)
	       getlines->control_handler(car);
	  getlines->escape = 0;
	  return 0;
     }
     switch (car) {
     case 0x08: /* Backspace. */
	  if(*bufpos) {
	       fputc(0x08,(FILE*)self);
	       fputc(' ' ,(FILE*)self);
	       fputc(0x08,(FILE*)self);
	       (*bufpos)--;
	  }
	  break;
     case '\n':
     case '\r':
	  buffer[*bufpos] = '\0';
	  *bufpos=0;
	  fputc('\r',(FILE*)self);
	  fputc('\n',(FILE*)self);
	  if(getlines->line_handler) getlines->line_handler(buffer);
	  break;
     case '\033':
	  buffer[*bufpos] = car;
	  (*bufpos)++;
	  break;
     case '[':
	  if(*bufpos>=1) {
	       if(buffer[*bufpos-1]=='\033') {
		    getlines->escape = 1;
		    (*bufpos)--;
		    return 0;
	       }
	  }
	  buffer[*bufpos] = car;
	  (*bufpos)++;
	  fputc(car,(FILE*)self);
	  break;
     default:
	  if(*bufpos==63) {

	  } else if (car >= 0x20 && car <= 0x7E) {
	       buffer[*bufpos] = car;
	       (*bufpos)++;
	       fputc(car,(FILE*)self);
	  } else if (car == '\t') {
	       buffer[*bufpos] = car;
	       (*bufpos)++;
	       fputc(' ',(FILE*)self);
	  }
	  break;
     }
     return 0;
}
void hrkos_getlines(FILE *fp,
		    void (*line_handler) (unsigned char *line),
		    void (*control_handler) (char ctrl) ) {
     /* Attach. */
     hrkos_file_t     *self = (hrkos_file_t *) fp;
     hrkos_getlines_t *getlines = malloc(sizeof(hrkos_getlines_t));
     self->fp.udata = getlines;
     /* Fill getlines. */
     getlines->bufpos = 0;
     getlines->line_handler = line_handler;
     getlines->control_handler = control_handler;
     getlines->escape = 0;
     /* Set new hook. */
     self->r_lock->func = (void*) hrkos_getlines_f;
     
}
