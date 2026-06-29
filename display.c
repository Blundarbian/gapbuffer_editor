#include <ncurses.h>	// includes stdio, unctrl, stdarg, stddef
			// TRUE, FALSE, OR, ERR
#include <stdlib.h>
#include <string.h>
#include "gap_buffer2.h"

// enum to hold init pairs
enum { PAIR_ERR = 1, PAIR_BW, PAIR_WB, PAIR_BR, PAIR_BB};

char splash_screen();
void resize_check();	
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

	resize_check();			// check srceen size
	getmaxyx(stdscr, maxy, maxx);

	if (argc == 2) 	// file in argument list 
	{		
		gb = copyfiletobuffer(argv[1]);
	}
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

	bkgd(COLOR_PAIR(PAIR_WB));			
	splash_screen();

	wbkgd(win, COLOR_PAIR(PAIR_BB));
	wmove(win, 0, 0);
	waddstr(win, gb->endgap);

	//box(stdscr, 0, 0);

	refresh();
	wrefresh(win);
	getch();

	endwin();
	return 0;
}


char splash_screen() 
{
	int c = '\0';
	int height = getmaxy(stdscr);
	int pos = height / 4;
	
	curs_set(0);
	center_rowaddstr(pos++, "Welcome to my Text editor!");
	center_rowaddstr(pos++, "This is a test of a menu screen");
	pos++;
	center_rowaddstr(pos++, "Created by me!");
	center_rowaddstr(pos++, "choose and option : o (open file), n (new file), q (quit)");
	refresh();

	char message[15] = "Key pressed (";		// show key if incorect option is pressed
	while (c != 'o' && c != 'n' && c != 'q')
	{
		c = getch();
		message[13] = c;
		message[14] = ')';
		message[15] = '\0';

		attron(A_BOLD);
		center_rowaddstr(pos++, message);	// print concat key string
		attroff(A_BOLD);

		move(pos--, 0);
		clrtoeol();			// move to front of line, clear, wait 50ms for next keypress
		napms(50);
	}

	curs_set(1);
	return c;
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


void resize_check() 
{
	int y = LINES, x = COLS;
	while (y < 40 || x < 40)
	{
		getmaxyx(stdscr, y, x);
		bkgd(COLOR_PAIR(PAIR_BR));
		move(0, 0);
		attron(A_BLINK);
		addstr("Please resize window\n");
		addstr("Must be > 40\n");
		attroff(A_BLINK);
		refresh();
		getch();
	}
	clear();
	refresh();
}
