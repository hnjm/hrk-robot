#ifndef _HRK_UART_C_
#define _HRK_UART_C_
#include <stdio.h>
#ifndef HRK_FIFO_LENGTH
#  define HRK_FIFO_LENGTH 32
#endif
typedef struct hrk_fifo_t {
     int  i,o;
     char buffer[HRK_FIFO_LENGTH];
     int  uart;
} hrk_fifo_t;
char  hrk_fifo_get(volatile hrk_fifo_t *fifo);
void  hrk_fifo_put(volatile hrk_fifo_t *fifo,char data);
FILE *hrk_uart_open(int uart,int baudrate);
void  hrk_uart_setbaud(int uart,unsigned long baud);
int   hrk_uart_putchar(char c,FILE *stream);
#endif /* _HRK_UART_C_ */
