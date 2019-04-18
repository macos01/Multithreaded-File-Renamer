CFLAGS  = -Wall -g -pthread
FUENTE  = main.c
PROG    = Renamer

all :
	gcc $(CFLAGS) -o $(PROG) $(FUENTE)

clean :
	-rm $(PROG)
