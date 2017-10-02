ARDUINO_HOME ?=
LIBARDUINO=-larduino-gemma
CC                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-gcc
CXX                   =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-g++
LD                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-ld
AR                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-ar
AVRDUDE               =$(ARDUINO_HOME)/hardware/tools/avr/bin/avrdude
CFLAGS               +=-std=c99 -mmcu=attiny85 -Os -DF_CPU=8000000L -DBAUD=9600 -I$(ARDUINO_HOME)/hardware/tools/avr/avr/include -I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/gemma
CXXFLAGS             +=-std=c++14 -mmcu=attiny85 -Os -DF_CPU=8000000L -DBAUD=9600 -I$(ARDUINO_HOME)/hardware/tools/avr/avr/include -I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/gemma
LDFLAGS              +=-mmcu=attiny85 -Os -L$(ARDUINO_HOME)/hardware/tools/avr/avr/lib/avr25
AVRDUDE_FLAGS        +=-C $(ARDUINO_HOME)/hardware/tools/avr/etc/avrdude.conf -p attiny85
LIBS                 +=-lattiny85
PORT = $(shell test ! -c /dev/ttyACM0 || echo /dev/ttyACM0)
SUDO = $(if $(PORT),$(shell test -w $(PORT) || echo sudo)) 
AVRDUDE_FLAGS += $(if $(PORT),-P $(PORT))
AVRDUDE_UPLOAD=$(SUDO) $(AVRDUDE) $(if $(PORT),) $(AVRDUDE_FLAGS) -Uflash:w:$(1):e
