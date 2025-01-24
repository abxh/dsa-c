EXEC_NAME := a.out

CC         := gcc
CFLAGS     += -I../../..
CFLAGS     += -Wall -Wextra -Wshadow -Wconversion -pedantic 
CFLAGS     += -ggdb3
CFLAGS     += -fsanitize=undefined
CFLAGS     += -fsanitize=address

C_FILES     := $(wildcard *.c)
OBJ_FILES   := $(C_FILES:.c=.o)

LD_FLAGS   += -fsanitize=undefined
LD_FLAGS   += -fsanitize=address

.PHONY: all clean test

all: $(EXEC_NAME)

clean:
	rm -rf $(OBJ_FILES)
	rm -rf $(EXEC_NAME)

test: $(EXEC_NAME)
	./a.out

$(EXEC_NAME): $(OBJ_FILES)
	$(CC) $(LD_FLAGS) $^ -o $(EXEC_NAME)

$(OBJ_FILES): $(SRC_FILES)

$(SRC_FILES):
	$(CC) -c $(CFLAGS) $@
