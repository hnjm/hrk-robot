#include "hrk-uart.h"
#include <util/delay.h>
int main (int argc,char *argv[]) {
     hrk_uart_open(0,9600);
     while(1) {
	  char c = fgetc(stdin);
	  fputc(c,stdout);
     }
}
