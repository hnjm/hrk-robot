CXXFLAGS             = -MMD -Os -mmcu=atmega2560 -D__AVR_ATmega2560__ -DBAUD=115200 -DF_CPU=16000000UL -I/home/harkaitz/.local/opt/arduino/hardware/tools/avr/avr/include -I/home/harkaitz/.local/opt/arduino/hardware/arduino/avr/cores/arduino -I/home/harkaitz/.local/opt/arduino/hardware/arduino/avr/variants/mega
CFLAGS               = -MMD -Os -mmcu=atmega2560 -D__AVR_ATmega2560__ -DBAUD=115200 -DF_CPU=16000000UL -I/home/harkaitz/.local/opt/arduino/hardware/tools/avr/avr/include -I/home/harkaitz/.local/opt/arduino/hardware/arduino/avr/cores/arduino -I/home/harkaitz/.local/opt/arduino/hardware/arduino/avr/variants/mega
RANLIB               = avr-ranlib
LD                   = avr-gcc
SYSROOT              = /home/harkaitz/.local/opt/arduino/tools/avr/atmega2560-16000000UL-mega
CPP                  = avr-cpp
CXX                  = avr-g++
LDFLAGS              = -L/home/harkaitz/.local/opt/arduino/tools/avr/atmega2560-16000000UL-mega/lib -mmcu=atmega2560
SUFFIX               = .elf
AVRDUDE_FLAGS        = -p m2560 -b 115200 -c stk500v2 -F -P /dev/ttyACM0
LIBS                 = -L/home/harkaitz/.local/opt/arduino/hardware/tools/avr/avr/lib/avr6 -latmega2560
ARDUINO_CORE         = /home/harkaitz/.local/opt/arduino/hardware/arduino/avr/cores/arduino
STRIP                = avr-strip
AR                   = avr-ar
AVRDUDE              = /usr/bin/avrdude
AS                   = avr-as
ARGS                 = 
PATH                 = /tmp/.hrkterm/tmpscripts:/media/harkaitz/HARKAITZ/Software/unix/bin:/home/harkaitz/.local/opt/netbeans/bin:/home/harkaitz/.local/opt/jet11.0-eval-amd64/bin:/home/harkaitz/.local/opt/jdk/bin:/home/harkaitz/.local/opt/jdk/jre/bin:/home/harkaitz/.local/opt/arduino/hardware/tools/avr/bin:/usr/local/bin:/usr/bin:/bin:/usr/local/games:/usr/games:/home/harkaitz/.local/bin:/home/harkaitz/.local/sbin:/home/harkaitz/.local/opt/android-sdk/tools:/home/harkaitz/.local/opt/android-sdk/platform-tools
MCU                  = atmega2560
PREFIX               = /home/harkaitz/.local/opt/arduino/tools/avr/atmega2560-16000000UL-mega
CC                   = avr-gcc
SIMULAVR_FLAGS       = -d atmega2560 -F 16000000UL
all:
BUILD=$(CC) -o $1 $2 $(CFLAGS) $(LDFLAGS) $(LIBS)
LINK=$(LD) $(LDFLAGS) $2 -o $1 $(LIBS)
%.o : %.c
	$(CXX) -c $< -o $@ $(CFLAGS)
clean: clean-objs
clean-objs:
	rm -f *.o *.d
distclean: clean
check-syntax:
	$(CC) -S $(CFLAGS) -o /tmp/null ${CHK_SOURCES} 
%.hex : %.elf
	avr-objcopy -O ihex $< $@
distclean: distclean-bins
distclean-bins:
	rm -f *.hex *.elf *.bin
$(PREFIX)/bin/%.elf: %.elf
	mkdir -p $(PREFIX)/bin
	cp $< $@
$(PREFIX)/bin/%.hex: %.hex
	mkdir -p $(PREFIX)/bin
	cp $< $@
