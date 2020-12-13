tarefas: tarefas.o
	gcc -o tarefas tarefas.o -Wl,-rpath . -L. -llpsolve55

tarefas.o: tarefas.c
	gcc -c tarefas.c -Wall