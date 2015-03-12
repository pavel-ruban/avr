MMCU = atmega48pa

link: assemble
	avr-ld avr.o -o avr.bin
assemble: avr.s
	avr-as -m $(MMCU) -g avr.s -o avr.o

gdb:
	simavr -m atmega48p -v -f 8000000 -g avr.bin

flash : link
	avrdude -Pusb -cusbasp -pm48p -U flash:w:avr.bin
