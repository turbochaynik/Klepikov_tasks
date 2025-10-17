CC = gcc
CFLAGS = -Wall -g
TARGET = program

SRC = main.c application.c

OBJ = $(SRC:.c=.o)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

%.o: %.c application.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJ) $(TARGET)
	
