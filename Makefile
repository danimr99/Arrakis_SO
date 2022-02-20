all: fremen clean

fremen.o: fremen.c generic_module.h fremen_module.h
	gcc -c fremen.c -Wall -Wextra

fremen: fremen.o generic_module.o fremen_module.o
	gcc fremen.o -o fremen -Wall -Wextra -ggdb3 -g

fremen_test:
	make
	clear
	./fremen fremen_config.txt

clean:
	rm -f *.o

clean_all:
	rm -f *.o fremen