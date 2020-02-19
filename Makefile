run: main.o cpu.o execute.o memory.o cpu.h
	gcc -g -o run main.o cpu.o execute.o memory.o

test: test.o cpu.o execute.o memory.o cpu.h
	gcc -g -o test test.o cpu.o execute.o memory.o

cpu.o: cpu.c
	gcc -g -c cpu.c
execute.o: execute.c
	gcc -g -c execute.c
memory.o: memory.c
	gcc -g -c memory.c
main.o: main.c
	gcc -g -c main.c
test.o: test.c
	gcc -g -c test.c

clean:
	rm test run test.o main.o cpu.o execute.o memory.o
