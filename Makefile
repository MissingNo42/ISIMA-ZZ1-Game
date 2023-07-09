CC = gcc
CFLAGS = -Wall -Wextra -Ofast
LDFLAGS = $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_gfx -lm -lSDL2 -lpthread

SRC = $(wildcard src/*.c)
RSRC=$(subst src/,,$(SRC))

HDR=$(addprefix build/,$(SRC:.c=.h))
OBJ=$(addprefix build/,$(RSRC:.c=.o))
DEP=$(addprefix build/,$(SRC:.c=.d))

all: main trainer

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c $<

main: $(SRC) $(OBJ) main.c
	$(CC) $(OBJ) main.c -o main $(CFLAGS) $(LDFLAGS)

#debug: $(SRC)
#	$(CC) main.c $(SRC) -o main -DTESTING=1 $(CFLAGS) $(LDFLAGS) -g

trainer: $(SRC) $(OBJ) training.c
	$(CC) $(OBJ) training.c -o trainer $(CFLAGS) $(LDFLAGS)

-include $(DEP)

.PHONY: nx run test clean reclean doc

nx:
	@rm -rf build
	@mkdir -p romfs/brains
	@cp -r assets romfs/
	@cp brains/*best* romfs/brains
	@cp main.c src/

	make -f Makefile.nx

	@rm -rf romfs
	@rm -rf build
	@rm *.elf
	@rm *.nacp
	@rm src/main.c

run:
	@printf "###### Run ######\n"
	@./main

test:
	@printf "###### Test ######\n"
	valgrind -s --leak-check=full --show-reachable=yes --error-limit=no --track-origins=yes --suppressions=./sdl.sup ./main

test_trainer:
	@printf "###### Test Trainer ######\n"
	valgrind -s --leak-check=full --show-reachable=yes --error-limit=no --track-origins=yes ./trainer

clean:
	rm -rf build *.o
	@rm -f src/main.c # if nx fail

reclean:
	rm -rf main trainer build *.o
	@rm -f src/main.c # if nx fail

doc:
	doxygen

