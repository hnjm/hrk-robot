ARDUINO_HOME ?=
LIBARDUINO=-larduino-atmega328-bt
CC                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-gcc
CXX                   =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-g++
LD                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-ld
AR                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-ar
AVRDUDE               =$(ARDUINO_HOME)/hardware/tools/avr/bin/avrdude
CFLAGS               +=-std=c99 -mmcu=atmega328p -Os -DF_CPU=16000000L -DBAUD=9600 -I$(ARDUINO_HOME)/hardware/tools/avr/avr/include -I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/eightanaloginputs
CXXFLAGS             +=-std=c++14 -mmcu=atmega328p -Os -DF_CPU=16000000L -DBAUD=9600 -I$(ARDUINO_HOME)/hardware/tools/avr/avr/include -I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/eightanaloginputs
LDFLAGS              +=-mmcu=atmega328p -Os -L$(ARDUINO_HOME)/hardware/tools/avr/avr/lib/avr5
AVRDUDE_FLAGS        +=-C $(ARDUINO_HOME)/hardware/tools/avr/etc/avrdude.conf -p m328p -b 19200 -c arduino
LIBS                 +=-latmega328p
PORT = $(shell test ! -c /dev/ttyACM0 || echo /dev/ttyACM0)
SUDO = $(if $(PORT),$(shell test -w $(PORT) || echo sudo)) 
AVRDUDE_FLAGS += $(if $(PORT),-P $(PORT))
AVRDUDE_UPLOAD=$(SUDO) $(AVRDUDE) $(if $(PORT),) $(AVRDUDE_FLAGS) -Uflash:w:$(1):e
