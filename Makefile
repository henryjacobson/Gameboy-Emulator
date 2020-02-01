run: main.o cpu.o execute.o memory.o cpu.h
	gcc -o run main.o cpu.o execute.o memory.o
main.o: main.c cpu.h execute.h memory.h 
	gcc -c main.c
cpu.o: cpu.c execute.h
	gcc -c cpu.c 
execute.o: execute.c cpu.h
	gcc -c execute.c
memory.o: memory.c 
	gcc -c memory.c
	
clean:
	rm run main.o cpu.o execute.o memory.o
