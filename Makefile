CC = gcc
CFLAGS = -Wall -Wextra -g
RCFLAGS = -Wall -Wextra -Ofast
LDFLAGS = $(sdl2-config --cflags --libs) -lSDL2_image -lSDL2_ttf -lSDL2_gfx -lm -lSDL2 -lpthread

SRC = $(wildcard *.c)

OBJ=$(addprefix build/,$(SRC:.c=.o))
DEP=$(addprefix build/,$(SRC:.c=.d))

all: $(OBJ)
	$(CC) -o main $^ $(LDFLAGS)

build/%.o: %.c
	@mkdir -p build
	$(CC) $(CFLAGS) -o $@ -c $<

release: $(SRC)
	$(CC) $(SRC) -o main $(RCFLAGS) $(LDFLAGS)

debug: $(SRC)
	$(CC) $(SRC) -o main -DTESTING=1 $(CFLAGS) $(LDFLAGS)

trainer: $(SRC)
	$(CC) $(SRC) -o trainer -DTRAINING=1 $(RCFLAGS) $(LDFLAGS)

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

testt:
	@printf "###### Test Trainer ######\n"
	valgrind -s --leak-check=full --show-reachable=yes --error-limit=no --track-origins=yes --suppressions=./sdl.sup ./trainer

clean:
	rm -rf build *.o

reclean:
	rm -rf main build *.o

doc:
	doxygen

