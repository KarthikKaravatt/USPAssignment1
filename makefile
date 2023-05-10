CC = gcc
LD = gcc
CFLAGS = -g 
LFLAGS = -lm -s
OBJ = main.o 
EXEC = calculator
$(EXEC): $(OBJ)
	$(LD) $(LFLAGS) $(OBJ) -o $(EXEC)

main.o: main.c 
	$(CC) -c main.c


clean:
	$(RM) $(EXEC) $(OBJ)

val:
	valgrind --leak-check=full -s ./$(EXEC)

# DO NOT DELETE
