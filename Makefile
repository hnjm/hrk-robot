TARGET=atmega2560-mega
include hrka/lib/mk/$(TARGET).mk
PREFIX=$(HOME)/.local
SYSROOT=$(PREFIX)/avr
EXECUTABLE=hrk-robot-$(TARGET).elf
LIBS    += $(LIBARDUINO)
LDFLAGS += -Lhrka/lib
SOURCES += main.c hrk-uart.c

$(EXECUTABLE) : $(SOURCES)
	-hrkproj update
	$(CC) -o $@ $(SOURCES) $(CFLAGS) $(LDFLAGS) $(LIBS)
install: $(SYSROOT)/avr/bin/$(EXECUTABLE)
$(SYSROOT)/avr/bin/$(EXECUTABLE) : $(EXECUTABLE)
	mkdir -p $(shell dirname $@)
	cp $< $@
upload: $(EXECUTABLE)
	$(call AVRDUDE_UPLOAD,$<) -v
check-syntax:
	$(CC) -S -o /dev/null ${CHK_SOURCES} $(CFLAGS)
test:
	sudo gtkterm --port $(PORT)
