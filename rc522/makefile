#############################################################
#######  MMC Atemga48pa make scripts by Pavel Ruban  ########
#############################################################

MMCU = atmega48
AMMCU = atmega48
FMMCU = m48

#MMCU = atmega48pa
#AMMCU = atmega48pa
#FMMCU = m48p
# Translate & link binary file
link: assemble compile
	avr-ld avr.o main.o spi.o mfrc522.o -T ld.cls -o avr.bin -lm -L/usr/avr/lib
	#avr-ld avr.o -T ld.cls -o avr.bin
assemble: avr.s
	avr-as -m $(MMCU) avr.s -I/sources/avr -o avr.o

link-test: assemble-test
	avr-ld avr_test.o -T ld.cls -o avr_test.bin

assemble-test: avr_test.s
	avr-as -m $(MMCU) avr_test.s -I/sources/avr -o avr_test.o

compile:	main.c lib/spi.c lib/mfrc522.c
	avr-gcc -O1 -mmcu=$(MMCU) -c main.c -o main.o -Iinclude
	avr-gcc -O1 -mmcu=$(MMCU) -c lib/spi.c -o spi.o -Iinclude
	avr-gcc -O1 -mmcu=$(MMCU) -c lib/mfrc522.c -o mfrc522.o -Iinclude -DF_CPU=8000000UL

obj: link
	avr-objdump -s avr.bin | less

objx: link
	avr-objdump -x avr.bin | less

# Run avr emulator with gdb server on 1234 port
sim: link
	simavr -m $(AMMCU) -v -f 8000000 -g avr.bin

# Connect gdb to simulator for runtime debuggin
gdb: link
	avr-gdb --ex='layout regs' --ex='target remote terminal:1234' -ex='focus CMD' --ex='symbol-file avr.bin'

# Update mcrocontroller firmware
flash : link
	avrdude -Pusb -cusbasp -p$(FMMCU) -U flash:w:avr.bin -s

flash-test : link-test
	avrdude -Pusb -cusbasp -p$(FMMCU) -U flash:w:avr_test.bin -s
