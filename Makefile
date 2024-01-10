all:
	gcc -o main main.c
	gcc -o fib fib.c
	gcc -o helloworld helloworld.c

clean:
	-@rm -f helloworld fib main