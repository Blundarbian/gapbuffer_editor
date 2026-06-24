# -*- MakeFile -*-
#target: dependencies
#	action 

display: gap_buffer2.o display.o
	gcc gap_buffer2.o display.o -lncurses -Wall -Wextra -o display  

gap_buffer2.o: gap_buffer2.c gap_buffer2.h
	gcc -c -Wall -Wextra gap_buffer2.c

display.o: display.c display.h
	gcc -c -Wall -Wextra display.c

clean:
	rm -f *.o display 
