#include <ncurses.h>	// includes stdio, unctrl, stdarg, stddef
			// TRUE, FALSE, OR, ERR
#include <stdlib.h>
#include <string.h>
#include "gap_buffer2.h"
#include "display.h"

#define NAME_SIZE 256
enum { PAIR_RW = 1, PAIR_BW, PAIR_WB, PAIR_BR, PAIR_BB};

char splash_screen();					// DONE : welcome screen when no file is provided 
void center_rowaddstr(int row, char *title);		// DONE	: places string centered along given row

void check_color();					// DONE : start color pairs
void check_resize();					// DONE	: Changes stdscr for size while waiting for proper resize
void check_gapbuffer(gap_buffer *gb);			// DONE : exit if gap_buffer dne
void check_window(WINDOW *win);				// DONE	: exit if window dne
void modeline(char mode, char c, int x, int y, size_t pos, gap_buffer *gb); 

int main(int argc, char *argv[])
{	
	int c, maxy, maxx;
	char name[NAME_SIZE];

	gap_buffer *gb;
	WINDOW *win;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	getmaxyx(stdscr, maxy, maxx);

	check_color();

	check_resize();			// check srceen size
	getmaxyx(stdscr, maxy, maxx);

	if (argc == 2) 	// file in argument list 
		gb = copyfiletobuffer(argv[1]);
	else
		while (1)
		{
			c = splash_screen();				// TODO : char splash input
			napms(50);		// nap between input
			if (c == 'n') 
			{
				gb = initgapbuffer(0);
				break;
			}
			else if (c == 'o')
			{
				getnstr(name, NAME_SIZE);
				gb = copyfiletobuffer(name);
				break;
			}
			else if (c == 'q')
			{
				endwin();
				exit(EXIT_SUCCESS);
			}
		}

	check_gapbuffer(gb);

	win = newwin(maxy - 2, maxx - 2, 1, 1);		// window check
	check_window(win);

	bkgd(COLOR_PAIR(PAIR_BW));
	wbkgd(win, COLOR_PAIR(PAIR_RW));

	int bufcount = 0;
	int endcount = (maxy - 2) * (maxx - 2);
	int x, y;
	size_t pos = 0;
	x = y = 0;

	shift_down(gb);
	shift_down(gb);
	shift_down(gb);
	shift_down(gb);
	x+=4;
	bufcount+=4;
	waddnstr(win, gb->buffer, bufcount);
	waddnstr(win, gb->endgap, endcount); 
	wmove(win, y, x);
	modeline('i', getch(), x, y, pos, gb);

	refresh();
	wrefresh(win);
	getch();

	endwin();
	return 0;
}


void modeline(char mode, char c, int x, int y, size_t pos, gap_buffer *gb) 
{
	move(LINES - 1, 1);
	clrtoeol();
	printw("mode : %s ", (mode == 'n') ? "[normal]" : "[insert]");
	printw("X: %d, Y: %d, %zu%% %c", x, y, pos / (gb->index + 1), c);
}

char splash_screen() 
{
	int c = '\0';
	int height, width;
	getmaxyx(stdscr, height, width);
	int pos = height / 4;

	curs_set(0);
	center_rowaddstr(pos++, "Welcome to my Text editor!");
	center_rowaddstr(pos++, "This is a test of a menu screen");
	center_rowaddstr(pos++, "Created by me!");
	pos++;

	attron(A_UNDERLINE);
	center_rowaddstr(pos++, "Choose an option");
	attroff(A_UNDERLINE);
	pos++;

	center_rowaddstr(pos++, "\tc (change colors)");
	center_rowaddstr(pos++, "\to (open file)    ");
	center_rowaddstr(pos++, "\tn (new file)     ");
	center_rowaddstr(pos++, "\tq (quit)         ");

	pos++;
	while ((c = getch()) && c != 'c' && c != 'o' && c != 'n' && c != 'q')
	{
		attron(A_BOLD);
		mvaddch(pos, width / 2, c); 
		attroff(A_BOLD);
		clrtoeol();			// move to front of line, clear, wait 50ms for next keypress
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


void check_color()
{
	if (!has_colors()) 
	{
		endwin();
		printf("error: terminal does not have color\n");
		exit(EXIT_FAILURE);
	}
	start_color();					
	init_pair(PAIR_RW, COLOR_RED, COLOR_WHITE);	// name, text color, background
	init_pair(PAIR_BW, COLOR_WHITE, COLOR_BLACK);
	init_pair(PAIR_WB, COLOR_BLACK, COLOR_WHITE);
	init_pair(PAIR_BR, COLOR_BLACK, COLOR_RED);
	init_pair(PAIR_BB, COLOR_WHITE, COLOR_BLUE);
}


void check_resize() 
{
	int y = LINES, x = COLS;
	while (y < 20 || x < 30)
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
	bkgd(COLOR_PAIR(PAIR_BW));
	refresh();
}


void check_gapbuffer(gap_buffer *gb)
{
	if (!gb)					// gap_buffer2 check
	{
		endwin();
		printf("error: gap_buffer cannot be created\n");
		exit(EXIT_FAILURE);
	}
}

void check_window(WINDOW *win)
{
	if (!win)
	{
		endwin();
		printf("error: window win cannot be created\n");
		exit(EXIT_FAILURE);
	}
}
