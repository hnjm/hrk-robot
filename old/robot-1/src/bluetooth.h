#ifndef _HRK_BLUETOOTH_H_
#define _HRK_BLUETOOTH_H_
#include "config.h"
#include <util/delay.h>
#include <stdio.h>
typedef struct {
  FILE_UART     fp;
  char          type;
  char          pwr_pin;
  char          code_pin;
  unsigned long baud;
} FILE_BLUETOOTH_HC5;
#define file_bluetooth_configurable_p(SELF) ((SELF)->pwr_pin && (SELF)->code_pin)
int  file_bluetooth_hc5_off     (FILE_BLUETOOTH_HC5 *self);
int  file_bluetooth_hc5_config  (FILE_BLUETOOTH_HC5 *self);
int  file_bluetooth_hc5_on      (FILE_BLUETOOTH_HC5 *self);
int  file_bluetooth_hc5_command (FILE_BLUETOOTH_HC5 *self,char *command,FILE *opt_output);
void file_bluetooth_hc5_init    (FILE_BLUETOOTH_HC5 *self,
				 int uart,unsigned long baud,
				 char opt_pwr_pin,char opt_code_pin,
				 const char *start_flag);

#ifndef BLUETOOTH_PWR_ON
#  define BLUETOOTH_PWR_ON(S)  ({     \
      pinMode((S)->pwr_pin,OUTPUT);   \
      digitalWrite((S)->pwr_pin,LOW); \
      _delay_ms(200);                 \
    })
#endif
#ifndef BLUETOOTH_PWR_OFF
#  define BLUETOOTH_PWR_OFF(S) ({      \
      pinMode((S)->pwr_pin,OUTPUT);   \
      digitalWrite((S)->pwr_pin,HIGH); \
      _delay_ms(200);                  \
    })
#endif
#ifndef BLUETOOTH_CODE_ON
#  define BLUETOOTH_CODE_ON(S)  ({	\
      pinMode((S)->code_pin,OUTPUT);	\
      digitalWrite((S)->code_pin,HIGH); \
    })
#endif
#ifndef BLUETOOTH_CODE_OFF
#  define BLUETOOTH_CODE_OFF(S) digitalWrite((S)->code_pin,LOW)
#endif

#ifdef HAS_ROBOT
FILE_BLUETOOTH_HC5 *
robot_bt_init(int uart,unsigned long baud,char opt_pwr_pin,char opt_code_pin,const char *start_flag);
#endif

#endif
