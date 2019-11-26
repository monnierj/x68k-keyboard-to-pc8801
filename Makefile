CC=avr-gcc
RM=rm
TARGET_MCU=attiny25
FIRMWARE_FILE=x68k-kbd-to-pc8801.elf
OBJECT_FILES=src/main.o

all: $(FIRMWARE_FILE)

$(FIRMWARE_FILE): $(OBJECT_FILES)
	$(CC) -o $(FIRMWARE_FILE) -mmcu=$(TARGET_MCU) $^

src/%.o: src/%.c
	$(CC) -c -o $@ -mmcu=$(TARGET_MCU) $<

clean:
	$(RM) -f $(OBJECT_FILES) $(FIRMWARE_FILE)