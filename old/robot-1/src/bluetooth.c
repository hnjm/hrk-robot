#include <Arduino.h>
#include "uart.h"
#include "bluetooth.h"
#ifdef HAS_ROBOT
#include "robot.h"
#endif



int file_bluetooth_hc5_off(FILE_BLUETOOTH_HC5 *self) {
  BLUETOOTH_PWR_OFF(self);
  BLUETOOTH_CODE_OFF(self);
  return 0;
}
int file_bluetooth_hc5_config(FILE_BLUETOOTH_HC5 *self) {
  if(!file_bluetooth_configurable_p(self)) return -1;
  BLUETOOTH_PWR_OFF(self);
  uart_setbaud((FILE_UART*)self,38400);
  BLUETOOTH_CODE_ON(self);
  BLUETOOTH_PWR_ON(self);
}
int file_bluetooth_hc5_on(FILE_BLUETOOTH_HC5 *self) {
  BLUETOOTH_PWR_OFF(self);
  BLUETOOTH_CODE_OFF(self);
  uart_setbaud((FILE_UART*)self,self->baud);
  BLUETOOTH_PWR_ON(self);
  return 0;
}
int file_bluetooth_hc5_command(FILE_BLUETOOTH_HC5 *self,char *command,FILE *opt_output) {
  int i; char buffer[64];
  fputs(command,(FILE*)self);
  i = 0;
  while(1) {
    if(fgets(buffer,sizeof(buffer),(FILE*)self)) {
      if(opt_output) {
	fputs(": ",opt_output);
	fputs(buffer,opt_output);
      }
      if(buffer[0]=='O') { break;         }
      if(buffer[1]=='F') { i = -1; break; }
    }
  }
  return i;
}

void file_bluetooth_hc5_init(FILE_BLUETOOTH_HC5 *self,
			     int uart,unsigned long baud,
			     char opt_pwr_pin,
			     char opt_code_pin,
			     const char *start_flag) {
  /* Configure pins. */
  if(uart!=-1) {
    self->pwr_pin  = opt_pwr_pin;
    self->code_pin = opt_code_pin;
    self->baud     = baud;
  }
  /* By default, put it off.*/
  file_bluetooth_hc5_off(self);
  /* Initialize UART. */
  if(uart==-1) {
    uart_setbaud((FILE_UART*)self,baud);
  } else {
    uart_init((FILE_UART*)self,uart,baud,start_flag);
  }
}
#ifdef HAS_ROBOT
static FILE_BLUETOOTH_HC5 hc5;

int robot_bt_config(int argc,char *argv[]) {
  del_object("@bt");
  add_object("@btc",&hc5);
  return file_bluetooth_hc5_config(&hc5);
}
int robot_bt_on(int argc,char *argv[]) {
  del_object("@btc");
  add_object("@bt",&hc5);
  return file_bluetooth_hc5_on(&hc5);
}
int robot_bt_off(int argc,char *argv[]) {
  del_object("@bt");
  del_object("@btc");
  return file_bluetooth_hc5_off(&hc5);
}
int robot_bt_run (int argc,char *argv[]) {
  char command[64]; int i;
  if(!get_object("@btc",NULL)) {
    robot_bt_config(0,NULL);
  }
  argv[0] = (char*) "AT";
  for(i=0,command[0]='\0';i<argc;) {
    strcat(command,argv[i]); i++;
    strcat(command,(i==argc)?"\r\n":"+");
  }
  for(i=0;command[i];i++) {
    command[i] = toupper(command[i]);
  }
  i = file_bluetooth_hc5_command(&hc5,command,stdout);
  return i;
}
int robot_bt_tty (int argc,char *argv[]) {
  static tty_t info;
  del_hook("bmain");
  memset(&info,0,sizeof(info));
  info.fp     = (FILE_UART*) &hc5;
  info.flags |= TTY_USE_PROMPT;
  add_hook(( int (*) (void*))tty,&info,"bmain");
}
static const struct robot_s commands [] = {
  {"btrun","COMMAND",robot_bt_run},
  {"btcfg",": Enter in config mode.",robot_bt_config},
  {"bton","Enter in normal mode.",robot_bt_on},
  {"btoff",": Stop bluetooth device.",robot_bt_off},
  {"btty"    ,"",robot_bt_tty},
  {NULL,NULL,NULL}
};
FILE_BLUETOOTH_HC5 *robot_bt_init(int uart,unsigned long baud,char opt_pwr_pin,char opt_code_pin,
				  const char *start_flag) {
  file_bluetooth_hc5_init(&hc5,uart,baud,opt_pwr_pin,opt_code_pin,start_flag);
  add_commands(commands);
  return &hc5;
}
#endif
