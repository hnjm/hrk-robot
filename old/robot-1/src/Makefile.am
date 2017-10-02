bin_PROGRAMS = robot
robot_SOURCES=bluetooth.h coreutils.h pwm.h robot.h uart.h bluetooth.c coreutils.c main.c pwm.c robot.c uart.c

check-syntax:
	$(COMPILE) -o /dev/null -S ${CHK_SOURCES}
