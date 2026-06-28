#include <ncurses.h>	// includes stdio, unctrl, stdarg, stddef
			// TRUE, FALSE, OR, ERR
#include <stdlib.h>
#include <string.h>
#include "gap_buffer2.h"

// enum to hold init pairs
enum { PAIR_ERR = 1, PAIR_BW, PAIR_WB, PAIR_BR, PAIR_BB};

int splash_screen();
void center_rowaddstr(int row, char *title);

int main(int argc, char *argv[])
{	
	gap_buffer *gb;
	WINDOW *win;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	int maxy, maxx;
	getmaxyx(stdscr, maxy, maxx);

	if (!has_colors()) 				// color check
	{
		fprintf(stderr, "error: terminal does not have color");
		endwin();
		return 1;
	}
	start_color();
	init_pair(PAIR_ERR, COLOR_RED, COLOR_WHITE);	// name, background, text color
	init_pair(PAIR_BW, COLOR_WHITE, COLOR_BLACK);
	init_pair(PAIR_WB, COLOR_BLACK, COLOR_WHITE);
	init_pair(PAIR_BR, COLOR_BLACK, COLOR_RED);
	init_pair(PAIR_BB, COLOR_CYAN, COLOR_BLACK);

	if (argc == 2)					// file in argument list
		gb = copyfiletobuffer(argv[1]);
	else
		gb = initgapbuffer(0);
	
	if (!gb)					// gap_buffer2 check
	{
		fprintf(stderr, "error: gap_buffer cannot be created");
		endwin();
		return 1;
	}


	win = newwin(maxy - 2, maxx - 2, 1, 1);		// window check
	if (!win)
	{
		fprintf(stderr, "error: window win cannot be created");
		endwin();
		return 1;
	}

	//splash_screen();
	center_rowaddstr(4, "Best programer ever!!!");
	refresh();
	getch();

	bkgd(COLOR_PAIR(PAIR_WB));			
	wbkgd(win, COLOR_PAIR(PAIR_BB));
	wrefresh(win);
	refresh();
	getch();

	wmove(win, 0, 0);
	waddstr(win, gb->endgap);

	//box(stdscr, 0, 0);

	refresh();
	wrefresh(win);
	getch();

	endwin();
	return 0;
}

int splash_screen() 
{

}

void center_rowaddstr(int row, char *title)
{
	int len, indent, width;

	width = getmaxx(stdscr);

	len = strlen(title);
	indent = (width - len) / 2;

	mvaddstr(row, indent, title);
	refresh();	
}
