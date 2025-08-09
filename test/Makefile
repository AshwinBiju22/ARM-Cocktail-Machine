CC      := gcc
CFLAGS  := -std=c17 -g -Wall -Werror -pedantic -O2 -I../src

TEST_SRCS := $(wildcard *.c)
TEST_BINS := $(TEST_SRCS:.c=.out)

EMU_OBJS  := ../src/emu_memory.o ../src/bit_utils.o ../src/emu_registers.o ../src/symbol_table.o ../src/instructions.o ../src/fde.o ../src/instructions.o

all: $(TEST_BINS)

%.out: %.c $(EMU_OBJS)
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(TEST_BINS)
