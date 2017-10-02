ARDUINO_HOME ?=
LIBARDUINO=-larduino-circuitplay32u4cat
CC                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-gcc
CXX                   =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-g++
LD                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-ld
AR                    =$(ARDUINO_HOME)/hardware/tools/avr/bin/avr-ar
AVRDUDE               =$(ARDUINO_HOME)/hardware/tools/avr/bin/avrdude
CFLAGS               +=-std=c99 -mmcu=atmega32u4 -Os -DF_CPU=8000000L -DBAUD=9600 -I$(ARDUINO_HOME)/hardware/tools/avr/avr/include -I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/circuitplay32u4 -DUSB_VID=0x239A -DUSB_PID=0x8011
CXXFLAGS             +=-std=c++14 -mmcu=atmega32u4 -Os -DF_CPU=8000000L -DBAUD=9600 -I$(ARDUINO_HOME)/hardware/tools/avr/avr/include -I$(ARDUINO_HOME)/hardware/arduino/avr/cores/arduino -I$(ARDUINO_HOME)/hardware/arduino/avr/variants/circuitplay32u4 -DUSB_VID=0x239A -DUSB_PID=0x8011
LDFLAGS              +=-mmcu=atmega32u4 -Os -L$(ARDUINO_HOME)/hardware/tools/avr/avr/lib/avr5
AVRDUDE_FLAGS        +=-C $(ARDUINO_HOME)/hardware/tools/avr/etc/avrdude.conf -p m32u4 -b 57600 -c avr109
LIBS                 +=-latmega32u4
PORT = $(shell test ! -c /dev/ttyACM0 || echo /dev/ttyACM0)
SUDO = $(if $(PORT),$(shell test -w $(PORT) || echo sudo)) 
AVRDUDE_FLAGS += $(if $(PORT),-P $(PORT))
AVRDUDE_UPLOAD=$(SUDO) $(AVRDUDE) $(if $(PORT),) $(AVRDUDE_FLAGS) -Uflash:w:$(1):e
