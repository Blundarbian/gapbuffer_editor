#include <ncurses.h>	// includes stdio, unctrl, stdarg, stddef, TRUE, FALSE, OR, ERR
#include <stdlib.h>
#include <string.h>
#include "gap_buffer2.h"
#include "display.h"

#define NAME_SIZE 256

void check_color();					// DONE : start color pairs
void check_resize();					// DONE	: Changes stdscr for size while waiting for proper resize
void check_gapbuffer(gap_buffer *gb);			// DONE : exit if gap_buffer dne
void check_window(WINDOW *win);				// DONE	: exit if window dne

	
typedef struct rendered_screen {

	int available;
	int highlight;
	char *screen;
	gap_buffer *gb;

} render;

enum HL_FORMATS {
	HL_NORMALS,
	HL_COMMENT,
	HL_KEYWORD,
	HL_STRINGS,
	HL_LITERAL,
	HL_SEARCHS
};


void check_color()
{
	if (!has_colors()) 
	{
		endwin();
		printf("error: terminal does not support color\n");
		exit(EXIT_FAILURE);
	}

	start_color();					
	init_pair(HL_NORMALS, COLOR_WHITE, COLOR_BLACK);	// name, text color, background
	
	init_color(COLOR_GRAY, 500, 500, 500);
	init_pair(HL_COMMENT, COLOR_GRAY, COLOR_WHITE);

	init_pair(HL_KEYWORD, COLOR_BLACK, COLOR_BLACK);
	init_pair(HL_STRINGS, COLOR_BLACK, COLOR_BLACK);
	init_pair(HL_LITERAL, COLOR_WHITE, COLOR_BLACK);
	init_pair(HL_SEARCHS, COLOR_WHITE, COLOR_MAGENTA);
}
	

int main(int argc, char *argv[])
{	
	render screen;

	initscr();
	noecho();
	keypad(stdscr, TRUE);

	getmaxyx(stdscr, info.maxy, info.maxx);

	check_color();

	check_resize();			// check srceen size
	getmaxyx(stdscr, info.maxy, info.maxx);

	if (argc == 2) 	// file in argument list 
	{
		gb = copyfiletobuffer(argv[1]);
		strncpy(info.name, argv[1], NAME_SIZE);
	}
	else
		while (1)
		{
			info.c = splash_screen();				// TODO : char splash input
			if (info.c == 'n') 
			{
				gb = initgapbuffer(0);
				strcpy(info.name, "--newfile--");
				break;
			}
			else if (info.c == 'o')
			{
				getnstr(info.name, NAME_SIZE);
				gb = copyfiletobuffer(info.name);
				break;
			}
			else if (info.c == 'q')
			{
				endwin();
				exit(EXIT_SUCCESS);
			}
		}
	check_gapbuffer(gb);

	endwin();
	return 0;
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
		addstr("min: 20x30\n");
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
