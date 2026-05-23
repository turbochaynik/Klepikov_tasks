CC = gcc
CFLAGS = -g -O0 -Wall
TARGET = shell
OBJECTS = tree.o list.o error.o shell.o

all: $(TARGET)

clean:
	rm -f *.o $(TARGET)

# Общее правило для компиляции .c в .o
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

# Зависимости
tree.o: tree.c tree.h list.h
list.o: list.c list.h
error.o: error.c error.h list.h
shell.o: shell.c tree.h list.h error.h

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET)

runvalg:
	rlwrap valgrind --leak-check=full ./$(TARGET)

run:
	rlwrap ./$(TARGET)