CC=gcc
CFLAGS=-c
all: task4
pwd.o: pwd.c
	$(CC) $(CFLAGS) pwd.c  -o pwd.o
ls.o: ls.c
	$(CC) $(CFLAGS) ls.c -o ls.o
echo.o: echo.c
	$(CC) $(CFLAGS) echo.c -o echo.o
cmd.o: cmd.c
	$(CC) $(CFLAGS) cmd.c -o cmd.o
cp.o: cp.c
	$(CC) $(CFLAGS) cp.c -o cp.o
tail.o: tail.c
	$(CC) $(CFLAGS) tail.c -o tail.o


task4: pwd.o ls.o echo.o cmd.o cp.o tail.o
	$(CC) -o pwd pwd.o 
	$(CC) ls.o -o ls
	$(CC) echo.o -o echo
	$(CC) cmd.o -o cmd
	$(CC) cp.o -o cp
	$(CC) tail.o -o tail
clean:
	rm -rf *.o
run:
	@echo "Enter command without ./:"
	@read RUN; ./$$RUN
