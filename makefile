CC=g++
CFLAGS = -g -Wall -c

all: Knapsack

Knapsack: Knapsack.o
	g++ Knapsack.cpp -o Knapsack
Knapsack.o: Knapsack.cpp
	$(CC) $(CFLAGS) Knapsack.cpp
clean: 
	rm -rf *o Knapsack
