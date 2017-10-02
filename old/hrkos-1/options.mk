TARGET=arduino-mega-atmega2560
LIBDIR=build/$(TARGET)/lib
BINDIR=build/$(TARGET)/bin
HEADIR=build/$(TARGET)/include/hrkos
CFLAGS=
CC      =HRKCC_BAUD=9600 hrkcc cc  --target=$(TARGET) 
LIB     =HRKCC_BAUD=9600 hrkcc lib --target=$(TARGET)
AVRDUDE =hrkcc avrdude --target=$(TARGET)
