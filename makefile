CC = gcc
LD = gcc
CFLAGS = -g 
LFLAGS = -g
OBJ = main.o 
EXEC = calculator
$(EXEC): $(OBJ)
	$(LD) $(LFLAGS) $(OBJ) -g -o $(EXEC)

main.o: main.c 
	$(CC) -g -c main.c


clean:
	$(RM) $(EXEC) $(OBJ)

val:
	valgrind --leak-check=full -s ./$(EXEC)

# DO NOT DELETE
