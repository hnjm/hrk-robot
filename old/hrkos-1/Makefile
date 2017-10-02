##
##
##
##
include options.mk

all: sh






##
## ** HRKOS **
##
HRKOS=\
  src/hrkos-sh.c src/hrkos-timer2.c src/hrkos.c
hrkos: $(LIBDIR)/libext.a $(LIBDIR)/libhrkos.a $(HEADIR)/hrkos.h
$(LIBDIR)/libext.a :
	$(LIB) $@ $(CFLAGS)
$(HEADIR)/hrkos.h : src/hrkos.h
	mkdir -p $(HEADIR)
	cp $< $(HEADIR)
$(LIBDIR)/libhrkos.a: $(HRKOS) src/hrkos.h
	$(LIB) $@ $(CFLAGS) $(HRKOS)



PROGRAM_CFLAGS =-I$(HEADIR)/.. $(CFLAGS)
PROGRAM_LIBS   =$(LIBDIR)/libhrkos.a 
##
## ** SH **
##
SH=programs/sh.c 
sh: $(BINDIR)/sh.elf
$(BINDIR)/sh.elf: $(SH) hrkos
	mkdir -p $(BINDIR)
	$(CC) $(PROGRAM_CFLAGS) -o $@ $(SH) $(PROGRAM_LIBS) $(LIBDIR)/libext.a




clean:
	rm -rf build
upload: $(BINDIR)/sh.elf
	$(AVRDUDE) -P /dev/ttyACM0 -U "flash:w:$(BINDIR)/sh.elf:e"
test:
	sudo gtkterm --port /dev/ttyACM0
