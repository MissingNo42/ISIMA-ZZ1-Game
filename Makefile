CC = gcc
CFLAGS = -Wall -Wextra -Ofast
LDFLAGS = $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_gfx -lm -lSDL2 -lpthread

SRC = $(wildcard *.c opti/*.c)

OBJ=$(addprefix build/,$(SRC:.c=.o))
DEP=$(addprefix build/,$(SRC:.c=.d))

all: $(OBJ)
	$(CC) -o main $^ $(LDFLAGS)

build/%.o: %.c
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c $<

-include $(DEP)

.PHONY: nx run test clean reclean doc

nx:
	make -f Makefile.nx

run:
	@printf "###### Run ######\n"
	@./main

test:
	@printf "###### Test ######\n"
	valgrind -s --leak-check=full --show-reachable=yes --error-limit=no --track-origins=yes --suppressions=./sdl.sup ./main

clean:
	rm -rf build *.o

reclean:
	rm -rf main build *.o

doc:
	doxygen
