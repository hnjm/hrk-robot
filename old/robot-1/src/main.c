#include <stdio.h>
#include <string.h>
#include "robot.h"
#include "uart.h"
#include "bluetooth.h"
#include "coreutils.h"
#include "pwm.h"
#include <avr/io.h>
tty_t info0; FILE_UART uart0; FILE_BLUETOOTH_HC5 *uart1;
int systemctl(void *a) {
  /* Check TTYs are working. */
  if(chk_hook("@main")==0) {
    fprintf(stderr,"Starting main tty ..." NL);
    uart_init(&uart0,0,9600,NULL);
    tty_add(&info0,&uart0,"@main");
  }
  /* Check Bluetooth is working. */
  if(chk_hook("bmain")==0) {
    fprintf(stderr,"Starting bluetooth tty ..." NL);
    robot_bt_init(1,9600,12,13,"START\r\n");
    robot("bton");
    robot("btty");
  }
  return 0;
}

int main (void) {
  uart_init(&uart0,0,9600,NULL);
  robot_init(&uart0);
  coreutils_init();
  fast_pwm_robot_init(FAST_PWM_1A|FAST_PWM_1B|FAST_PWM_2A|FAST_PWM_2B);
  fast_pwm_set(FAST_PWM_2A,255);
  fast_pwm_set(FAST_PWM_2B,255);
  add_hook(systemctl,NULL,"systemctl");
  /* Main hook. */
  while(1) {
    robot_hook();
  }
}
