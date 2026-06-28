#include <ncurses.h>	// includes stdio, unctrl, stdarg, stddef
			// TRUE, FALSE, OR, ERR
#include <stdlib.h>
#include <string.h>
#include "gap_buffer2.h"

// enum to hold init pairs
enum { PAIR_ERR = 1, PAIR_BW, PAIR_WB, PAIR_BR };

int main(int argc, char *argv[])
{	
	gap_buffer *gb;
	WINDOW *bar; 

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	int maxx, maxy;
	getmaxyx(stdscr, maxy, maxx);

	if (!has_colors()) 
	{
		fprintf(stderr, "error: terminal does not have color");
		endwin();
		return 1;
	}
	start_color();
	init_pair(PAIR_ERR, COLOR_RED, COLOR_WHITE);
	init_pair(PAIR_BW, COLOR_WHITE, COLOR_BLACK);
	init_pair(PAIR_WB, COLOR_BLACK, COLOR_WHITE);
	init_pair(PAIR_BR, COLOR_BLACK, COLOR_RED);

	if (argc == 2)	
		gb = copyfiletobuffer(argv[1]);
	else
		gb = initgapbuffer(0);
	
	if (!gb)
	{
		fprintf(stderr, "error: gap_buffer cannot be created");
		endwin();
		return 0;
	}

	bkgd(COLOR_PAIR(PAIR_BW));			
	mvaddstr(1, 1, "This is a testing\n");
	box(stdscr, 0, 0);
	refresh();
	getch();

	endwin();
	return 0;
}
