CC       ?= gcc
CFLAGS   ?= -std=c17 -g -O2 -Wall -Werror -pedantic -D_POSIX_SOURCE -D_DEFAULT_SOURCE
LIBS     := -lrt -pthread
INCLUDES := -Ishared -Iassembler -Iemulator -Iextension

.PHONY: all clean test-all tidy extension-test extension-rpi format debug-rpi

# Source file lists
ASSEMBLER_SRC := \
	assembler/assemble.c \
	assembler/instruction_assembler.c \
	assembler/encoder.c \
	assembler/symbol_table.c \
	assembler/parser.c \
	assembler/bin_writer.c

EMULATOR_SRC := \
	emulator/emulate.c \
	emulator/instructions.c \
	emulator/fde.c

SHARED_SRC := \
	shared/bit_utils.c \
	shared/emu_memory.c \
	shared/emu_registers.c

EXTENSION_SRC := \
    cocktail_maker/lcd_i2c.c \
    cocktail_maker/maker.c \

PI_DETECTED := $(shell grep -q 'Raspberry Pi' /proc/cpuinfo && echo 1 || echo 0)

ifeq ($(PI_DETECTED), 1)
    CFLAGS += -DON_PI
    LIBS += -lpigpio
else
    EXTENSION_SRC += cocktail_maker/pigpio_emu.c
endif

ASSEMBLER_OBJS := $(ASSEMBLER_SRC:.c=.o) $(SHARED_SRC:.c=.o)
EMULATOR_OBJS  := $(EMULATOR_SRC:.c=.o)  $(SHARED_SRC:.c=.o)
EXTENSION_OBJS := $(EXTENSION_SRC:.c=.o)

all: assemble emulate extension-test

assemble: $(ASSEMBLER_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

emulate: $(EMULATOR_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@

emu-extension: CFLAGS += -DEMU_OUTPUT
emu-extension: cocktailmaker

debug-extension: CFLAGS += -DDEBUG
debug-extension: cocktailmaker

cocktailmaker: $(EXTENSION_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	$(RM) assembler/*.o emulator/*.o shared/*.o assemble emulate *.o cocktail-maker cocktail_maker/*.o *.log
	@$(MAKE) -C ../test clean

test-all:
	./run_tests.sh

# Clang-Tidy linting

FORMAT := clang-format
TIDY := clang-tidy
TIDY_FLAGS := -- $(CFLAGS) $(INCLUDES)
ALL_SRC := $(ASSEMBLER_SRC) $(EMULATOR_SRC) $(SHARED_SRC)
FORMAT_SRC := $(ASSEMBLER_SRC) $(EMULATOR_SRC) $(SHARED_SRC) $(wildcard */*.h)

compile_commands.json:
	bear -- make clean all

tidy: compile_commands.json format
	@echo "Running clang-tidy on all .c files..."
	@for file in $(ALL_SRC); do \
		$(TIDY) $$file $(TIDY_FLAGS); \
	done

format: 
	@echo "Running clang-format on all files..."
	clang-format -i $(FORMAT_SRC)
