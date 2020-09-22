CC=gcc

INC += -I./fake_hardware

CFLAGS += -Demulator


run: ../src/main.c ../src/display.c ../src/display_print.c
	$(CC) -lSDL2 $(CFLAGS) $(INC) -o $@ $^

clean:
	rm *.o

