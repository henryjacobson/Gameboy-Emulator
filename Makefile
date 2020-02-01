run: main.o cpu.o execute.o memory.o cpu.h
	gcc -o run main.o cpu.o execute.o memory.o


cpu.o: cpu.c
	gcc -c cpu.c
execute.o: execute.c
	gcc -c execute.c
memory.o: memory.c
	gcc -c memory.c
main.o: main.c
	gcc -c main.c

clean:
	rm run main.o cpu.o execute.o memory.o
