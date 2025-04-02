CC = gcc
CFLAGS = -g -W -Wall 

all: part1 part2 part3 part4 iobound cpubound

part1: part1.o string_parser.o
	$(CC) part1.o string_parser.o -o part1 

part1.o: part1.c  
	$(CC) $(CFLAGS) -c part1.c 

part2: part2.o string_parser.o
	$(CC) part2.o string_parser.o -o part2 

part2.o: part2.c  
	$(CC) $(CFLAGS) -c part2.c 

part3: part3.o string_parser.o
	$(CC) part3.o string_parser.o -o part3 

part3.o: part3.c  
	$(CC) $(CFLAGS) -c part3.c 

part4: part4.o string_parser.o
	$(CC) part4.o string_parser.o -o part4 

part4.o: part4.c  
	$(CC) $(CFLAGS) -c part4.c 
	
string_parser.o: string_parser.c string_parser.h
	$(CC) $(CFLAGS) -c string_parser.c

iobound: iobound.c
	$(CC) $< $(CFLAGS) -o iobound

cpubound: cpubound.c
	$(CC) $< $(CFLAGS) -o cpubound

#command.o: command.c command.h
#	$(CC) $(CFLAGS) -c command.c

	
clean:
	rm -f *.o part1 part2 part3 part4 iobound cpubound
#	rm test/input.txt 
#	rmdir test 
#	rm output.txt
