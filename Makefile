all: fremen atreides harkonen clean

generic_module.o: generic_module.c generic_module.h
	gcc -c generic_module.c

frame_module.o: frame_module.c frame_module.h
	gcc -c frame_module.c

fremen_module.o: fremen_module.c fremen_module.h
	gcc -c fremen_module.c

fremen.o: fremen.c generic_module.h frame_module.h fremen_module.h
	gcc -c fremen.c -Wall -Wextra

fremen: fremen.o generic_module.o frame_module.o fremen_module.o
	gcc fremen.o generic_module.o frame_module.o fremen_module.o -o fremen -Wall -Wextra -ggdb3 -g

fremen_test:
	make
	clear
	./fremen fremen_config.dat

atreides_module.o: atreides_module.c atreides_module.h
	gcc -c atreides_module.c

atreides.o: atreides.c generic_module.h frame_module.h atreides_module.h
	gcc -c atreides.c -Wall -Wextra

atreides: atreides.o generic_module.o frame_module.o atreides_module.o
	gcc atreides.o generic_module.o frame_module.o atreides_module.o -o atreides -lpthread -Wall -Wextra -ggdb3 -g

atreides_test:
	make
	clear
	./atreides atreides_config.txt

harkonen_module.o: harkonen_module.c harkonen_module.h
	gcc -c harkonen_module.c

harkonen.o: harkonen.c generic_module.h harkonen_module.h
	gcc -c harkonen.c -Wall -Wextra

harkonen: harkonen.o generic_module.o harkonen_module.o
	gcc harkonen.o generic_module.o harkonen_module.o -o harkonen -Wall -Wextra -ggdb3 -g

harkonen_test:
	make
	clear
	./harkonen 5

clean:
	rm -f *.o

clean_all:
	rm -f *.o fremen atreides harkonen