SERIAL=/dev/tty.?
CPU_SPEED=16000000
MCU=atmega328
CFLAGS=-Os -DF_CPU=$(CPU_SPEED) -mmcu=$(MCU)
CC=avr-gcc
LDFLAGS=-DF_CPU=$(CPU_SPEED) -mmcu=$(MCU)

#implicit rules make main from main.c

main: main.o

main.hex: main
	avr-objcopy -O ihex main main.hex

hex: main.hex

flash: hex
	avrdude -c arduino -p $(MCU) -P $(SERIAL) -U flash main.hex

clean:
	rm main *.o *.hex
