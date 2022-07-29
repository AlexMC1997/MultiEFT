SRC = ./src
GCC_FLAGS = -O0 -g3 -fsanitize=address
LIBS = -lm
CC = gcc

all: $(SRC)/EFTFloat.c $(SRC)/test.c 
	$(CC) -o test $(GCC_FLAGS) $(SRC)/EFTFloat.c $(SRC)/test.c  $(LIBS)