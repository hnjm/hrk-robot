#include <hrkos/hrkos.h>


int main (int argc,char *argv[]) {
  FILE *fp = hrkos_uart_init(0,9600);
  fputs("Wellcome to the arduino shell" NL,fp);
  //hrkos_hook_blink(HOOK_CSEC,50);
  hrkos_hook_shell(fp);
  timer_20kHz_register(timer_20kHz_new_pwm(20,10,13));
  while(1) {
    hrkos_burner();
  }
}
