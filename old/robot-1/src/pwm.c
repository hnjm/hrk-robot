#include "pwm.h"
#include <avr/io.h>
#include <stdint.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#ifdef HAS_ROBOT
#  include "robot.h"
#endif

void fast_pwm_init(int flags) {
  if(flags & (FAST_PWM_1A|FAST_PWM_1B)) {
    /* Not tested. */
#   ifdef __AVR_ATmega2560__
    if(flags & FAST_PWM_1A) {
      PINB &= ~_BV(5);
      DDRB |= _BV(5);
    }
    if(flags & FAST_PWM_1B) {
      PINB &= ~_BV(6);
      DDRB |= _BV(6);
    }
#   endif
    TCCR1A=0b10100011; /* [COM<n>A{1,0}][COM<n>B{1,0}][R][R][WGT<n>{1,0}] */
    TCCR1B=0b00000101; /* [FOC<n>{A,B}][R][R][WGT<n>2][CS<n>{2,1,0}] */
  }
  if(flags & (FAST_PWM_2A|FAST_PWM_2B)) {
#   ifdef __AVR_ATmega2560__
    if(flags & FAST_PWM_2A) {
      PINB &= ~_BV(4);
      DDRB |= _BV(4);
    }
    if(flags & FAST_PWM_2B) {
      PINH &= ~_BV(6);
      DDRH |= _BV(6);
    }
#   endif
    TCCR2A=0b10100011; /* [COM<n>A{1,0}][COM<n>B{1,0}][R][R][WGT<n>{1,0}] */
    TCCR2B=0b00000101; /* [FOC<n>{A,B}][R][R][WGT<n>2][CS<n>{2,1,0}] */
  }
}
void fast_pwm_set(int flags,unsigned char value) {
  if(flags & FAST_PWM_1A) OCR1A = value;
  if(flags & FAST_PWM_1B) OCR1B = value;
  if(flags & FAST_PWM_2A) OCR2A = value;
  if(flags & FAST_PWM_2B) OCR2B = value;
}
#ifdef HAS_ROBOT
int fast_pwm_robot_set (int argc,char *argv[]) {
  int i;
  for(i=1;i<argc;i++) {
    char cfg;
#   define fast_pwm_robot_set_a 0x01
#   define fast_pwm_robot_set_b 0x02
#   define fast_pwm_robot_set_1 0x04
#   define fast_pwm_robot_set_2 0x08
    char *j;
    int flags; unsigned char d255;
    for(j=argv[i],cfg=0,d255=0,flags=0;(*j);j++) {
      switch(*j) {
      case '%':           d255 = (atoi(j+1))*(255.0/100);  break;
      case '1':           cfg |= fast_pwm_robot_set_1; break;
      case '2':           cfg |= fast_pwm_robot_set_2; break;
      case 'a': case 'A': cfg |= fast_pwm_robot_set_a; break;
      case 'b': case 'B': cfg |= fast_pwm_robot_set_b; break;
      }
      if(cfg & fast_pwm_robot_set_1) {
	if(cfg & fast_pwm_robot_set_a) flags |= FAST_PWM_1A;	
	if(cfg & fast_pwm_robot_set_b) flags |= FAST_PWM_1B;
      }
      if(cfg & fast_pwm_robot_set_2) {
	if(cfg & fast_pwm_robot_set_a) flags |= FAST_PWM_2A;
	if(cfg & fast_pwm_robot_set_b) flags |= FAST_PWM_2B;
      }
      if(*j=='%') {
	if(d255<=255) {
	  printf("PWM SET flags=%4x d255=%d" NL,flags,d255);
	  fast_pwm_set(flags,d255);
	}
	break;
      }
    }
  }
  
  return 0;
}
static struct robot_s commands[] = {
  {"pwm","[[1][2][A][B]%100][...]",fast_pwm_robot_set},
  {NULL,NULL,NULL}
};
void fast_pwm_robot_init(int flags) {
  fast_pwm_init(flags);
  add_commands(commands);
}
#endif
