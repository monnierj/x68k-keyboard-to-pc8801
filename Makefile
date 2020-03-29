CC=avr-gcc
RM=rm
TARGET_MCU=attiny25
FIRMWARE_FILE=x68k-kbd-to-pc8801.elf
OBJECT_FILES=src/main.o src/keymap.o

all: $(FIRMWARE_FILE)

$(FIRMWARE_FILE): $(OBJECT_FILES)
	$(CC) -o $(FIRMWARE_FILE) -O2 -mmcu=$(TARGET_MCU) $^

src/%.o: src/%.c
	$(CC) -c -o $@ -O2 -mmcu=$(TARGET_MCU) $<

clean:
	$(RM) -f $(OBJECT_FILES) $(FIRMWARE_FILE)
