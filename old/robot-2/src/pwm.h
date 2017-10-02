#ifndef _PWM_H_
#define _PWM_H_
#include "config.h"
void fast_pwm_init(int flags);
#define FAST_PWM_1A 0x01
#define FAST_PWM_1B 0x02
#define FAST_PWM_2A 0x04
#define FAST_PWM_2B 0x08
#define F_PWM (F_CPU/(1024*256))
void fast_pwm_set(int flags,unsigned char value);
#ifdef HAS_ROBOT
void fast_pwm_robot_init(int flags);
#endif

#endif
