ARDUINO_HOME ?=
LIBARDUINO=-larduino-megaADK
CC                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-gcc
CXX                   =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-g++
LD                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-ld
AR                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-ar
AVRDUDE               =$(ARDUINO_HOME)/hardware/tools/avr/bin/avrdude
CFLAGS               +=-std=c99 -mmcu=atmega2560 -Os -DF_CPU=16000000L -DBAUD=9600 -I$(ARDUINO_HOME)/hardware/tools/avr/avr/include -I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/mega
CXXFLAGS             +=-std=c++14 -mmcu=atmega2560 -Os -DF_CPU=16000000L -DBAUD=9600 -I$(ARDUINO_HOME)/hardware/tools/avr/avr/include -I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/mega
LDFLAGS              +=-mmcu=atmega2560 -Os -L$(ARDUINO_HOME)/hardware/tools/avr/avr/lib/avr6
AVRDUDE_FLAGS        +=-C $(ARDUINO_HOME)/hardware/tools/avr/etc/avrdude.conf -p m2560 -b 115200 -c wiring
LIBS                 +=-latmega2560
PORT = $(shell test ! -c /dev/ttyACM0 || echo /dev/ttyACM0)
SUDO = $(if $(PORT),$(shell test -w $(PORT) || echo sudo)) 
AVRDUDE_FLAGS += $(if $(PORT),-P $(PORT))
AVRDUDE_UPLOAD=$(SUDO) $(AVRDUDE) $(if $(PORT),) $(AVRDUDE_FLAGS) -Uflash:w:$(1):e
