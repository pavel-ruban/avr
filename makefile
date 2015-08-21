#############################################################
#######  MMC Atemga48pa make scripts by Pavel Ruban  ########
#############################################################

MMCU = atmega48p

# Translate & link binary file
link: assemble
	avr-ld avr.o -T ld.cls -o avr.bin
assemble: avr.S
	avr-gcc -mmcu=$(MMCU) -c avr.S -o avr.o

obj: link
	avr-objdump -s avr.bin | less

objx: link
	avr-objdump -x avr.bin | less

# Run avr emulator with gdb server on 1234 port
sim: link
	simavr -m atmega48p -v -f 8000000 -g avr.bin

# Connect gdb to simulator for runtime debuggin
gdb: link
	avr-gdb --ex='layout regs' --ex='target remote terminal:1234' -ex='focus CMD' --ex='symbol-file avr.bin'

# Update microcontroller firmware
flash : link
	avrdude -Pusb -cusbasp -pm48p -v -U flash:w:avr.bin
