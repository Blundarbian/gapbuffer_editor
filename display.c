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
	WINDOW *main, *bar, *menu;
	initscr();

	int max_y, max_x;
	getmaxyx(stdscr, max_y, max_x);

	if (!has_colors()) 
	{
		fprintf(stderr, "error: terminal does not have color\n");
		endwin();
		return 1;
	}
	start_color();
	init_pair(PAIR_ERR, COLOR_RED, COLOR_WHITE);
	init_pair(PAIR_BW, COLOR_WHITE, COLOR_BLACK);
	init_pair(PAIR_WB, COLOR_BLACK, COLOR_WHITE);
	init_pair(PAIR_BR, COLOR_BLACK, COLOR_RED);

	addstr("This is a testing\n");
	refresh();
	getch();


	endwin();
	return 0;
}
