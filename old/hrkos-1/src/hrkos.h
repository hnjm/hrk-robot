#ifndef _HRKOS_H_
#define _HRKOS_H_


/*****************************************************************
 * Operating system options.
 *****************************************************************/
#ifndef F_CPU
#  error "Please define F_CPU."
#endif
#ifndef BAUD
#  error "Please define BAUD."
#endif
#define NL "\r\n"
#define GREEN(TXT) "\033[32m" TXT "\033[39m"
#define YELLOW(TXT)   "\033[33m" TXT "\033[39m"
#define RED(TXT)   "\033[35m" TXT "\033[39m"

#include <stdio.h>
#include <setjmp.h>
#include <avr/io.h>

/*********************************************************
 * Lock mechanism.
 *********************************************************/
typedef struct {
     int     flags;
#    define HRKOS_LOCK_IS_USED   0x01
#    define HRKOS_LOCK_IS_LOCKED 0x02
#    define HRKOS_LOCK_FREE      0x04
#    define HRKOS_LOCK_TIMED     0x08
     int    (*check) (void *udata);
     int    (*func)  (void *udata);
     void    *udata;
} hrkos_lock_t;
hrkos_lock_t *hrkos_lock_new  (int (*check) (void *udata),void *udata,int (*func) (void *udata),int flags);
void          hrkos_lock_check(hrkos_lock_t *lock);


/*******************************************************
 * I/O
 *******************************************************/
typedef struct {
     FILE          fp;
     int           num;
     hrkos_lock_t *w_lock;
     hrkos_lock_t *r_lock;
} hrkos_file_t;

FILE *hrkos_uart_init(int num,unsigned long baud);
void  hrkos_burner(void);

/*******************************************************
 * IDLE time operations. (TIMER1)
 *******************************************************/
void hrkos_hook(int scale,int value,int (*fun) (void *ign));
void hrkos_hook_blink(int scale,int value);
#define HOOK_CSEC 0
#define HOOK_SEC  1
#define HOOK_MIN  2
#define HOOK_HOUR 3
#define HOOK_WEEK 4


/********************************************************
 * Interactive I/O
 ********************************************************/
void hrkos_getlines(FILE *fp,
		    void (*line_handler) (unsigned char *line),
		    void (*control_handler) (char ctrl));



/********************************************************
 * SHELL.
 ********************************************************/
void hrkos_hook_shell(FILE *fp);
extern int  system(const char *buffer);
void program(char *name,char *hlp,int (*code) (FILE *fp,int argc,char **argv));
#define LEFT_PROGRAM   "left"
#define RIGHT_PROGRAM  "right"
#define UP_PROGRAM     "up"
#define DOWN_PROGRAM   "down"


/*********************************************************
 * TIMER2 (20kHz clock)
 *********************************************************/
typedef struct {
     unsigned char counter;
     unsigned char value;
     unsigned char max;
     int (*match)    (void *udata);
     int (*overflow) (void *udata);
     char data[1];
} hrkos_timer_20kHz_t;
void timer_20kHz_register(hrkos_timer_20kHz_t *timer);
hrkos_timer_20kHz_t *timer_20kHz_new_pwm(unsigned char prescaler,unsigned char value,int pin);




#endif
