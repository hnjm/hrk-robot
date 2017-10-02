#include "hrkos.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>
#ifndef MAX_TIMER_20KHZ_OPERATIONS
#  define MAX_TIMER_20KHZ_OPERATIONS 20
#endif


static hrkos_timer_20kHz_t *timers[MAX_TIMER_20KHZ_OPERATIONS] = { 0 };
void timer_20kHz_burn(void) {
     int i = 0;
     
     
     for(i=0;i<MAX_TIMER_20KHZ_OPERATIONS;i++) {
	  hrkos_timer_20kHz_t *t = timers[i];
	  if(!t) continue;
	  if(t->overflow && t->counter == t->max) {
	       t->overflow(t->data);
	       t->counter = 0;
	  } else if(t->match && t->counter == t->value) {
	       t->match(t->data);
	       if(t->overflow) t->counter++;
	       else            t->counter=0;
	  } else {
	       t->counter++;
	  }
     }
}
ISR (TIMER1_COMPA_vect) {
     timer_20kHz_burn();
}

void timer_20kHz_register(hrkos_timer_20kHz_t *timer) {
     int i = 0; static int started = 0;
     if(!started) {
	  /*
	   * INTERRUPT_TIME=N/(F_CPU/8)=0.05ms (50us)
	   * N=0.05ms*(F_CPU/8)=F_CPU/8/100000*5
	   */
	  const unsigned int ovf_value = (F_CPU/8/100000*5);
	  OCR1A = ovf_value; /* ovf_value, 10kHz */
	  TCCR1A=0b00000000; /* [COM0A{1,0}][COM0B{1,0}][R][R][WGT0{1,0}] */
	  TCCR1B=0b00001010; /* [FOC0{A,B}][R][R][WGT02][CS0{2,1,0}] */
	  TIMSK1 |= (1 << OCIE0A); //Set the ISR COMPA vect
	  started = 1;
     }
     for(i=0;i<MAX_TIMER_20KHZ_OPERATIONS;i++) {
	  if(timers[i]) continue;
	  timers[i] = timer;
	  break;
     }
}

hrkos_timer_20kHz_t *timer_new_20kHz(int (*match)    (void *udata),
				     int (*overflow) (void *udata),
				     unsigned char value,
				     unsigned char max,
				     void *udata,int len) {
     hrkos_timer_20kHz_t *self = malloc(sizeof(*self)+len);
     self->counter  = 0;
     self->value    = value;
     self->max      = max;
     self->match    = match;
     self->overflow = overflow;
     memcpy(self->data,udata,len);
     return self;
}
int pin_set(int *pin) {
     pinMode(*pin,OUTPUT);
     digitalWrite(*pin,HIGH);
}
int pin_clr(int *pin) {
     pinMode(*pin,OUTPUT);
     digitalWrite(*pin,LOW);
}
hrkos_timer_20kHz_t *timer_20kHz_new_pwm(unsigned char prescaler,unsigned char value,int pin) {
     return timer_new_20kHz((void*)pin_clr,(void*)pin_set,value,prescaler,&pin,sizeof(pin));
}

