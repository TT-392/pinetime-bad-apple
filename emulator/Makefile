CC=gcc

INC += -I./fake_hardware

CFLAGS += -Demulator


run: main.c fake_hardware/display.c #../src/display_print.c
	$(CC) -lSDL2 $(CFLAGS) $(INC) -o $@ $^

clean:
	rm *.o

