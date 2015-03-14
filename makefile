#############################################################
#######  MMC Atemga48pa make scripts by Pavel Ruban  ########
#############################################################

MMCU = atmega48pa

# Translate & link binary file
link: assemble
	avr-ld avr.o -T ld.cls -o avr.bin
assemble: avr.s
	avr-as -m $(MMCU) -g3 avr.s -o avr.o

# Run avr emulator with gdb server on 1234 port
sim: link
	simavr -m atmega48p -v -f 8000000 -g avr.bin

# Connect gdb to simulator for runtime debuggin
gdb: link
	avr-gdb --symbols=/sources/avr/avr.bin --ex='layout regs' --ex='target remote terminal:1234'

# Update microcontroller firmware
flash : link
	avrdude -Pusb -cusbasp -pm48p -U flash:w:avr.bin
