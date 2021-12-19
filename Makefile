project4: db.o project4.o
	gcc -Wall -g db.o project4.o -o project4

db.o: db.c db.h
	gcc -Wall -g -c db.c

project4.o: project4.c db.h
	gcc -Wall -g -c project4.c
	
clean:
	rm *.o
