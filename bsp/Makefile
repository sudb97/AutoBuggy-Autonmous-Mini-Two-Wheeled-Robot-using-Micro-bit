include ../Makefile

BSPOBJS += adc.o audio.o board.o gpio.o lib.o pwm.o serial.o \
           startup.o system.o timer.o

# Dependencies

libbsp.a: $(BSPOBJS)
	$(AR) -r libbsp.a startup.o system.o board.o \
        serial.o gpio.o pwm.o adc.o lib.o audio.o timer.o

bspclean:
	/bin/rm $(BSPOBJS) libbsp.a

adc.o: adc.c adc.h
audio.o: audio.c audio.h gpio.h pwm.h adc.h lib.h
board.o: board.c board.h lib.h
gpio.o: gpio.c gpio.h board.h lib.h
lib.o: lib.c lib.h
pwm.o: pwm.c pwm.h
serial.o: serial.c gpio.h serial.h
startup.o: startup.c
system.o: system.c
timer.o: timer.c timer.h
