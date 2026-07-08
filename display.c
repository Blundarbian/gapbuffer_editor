#include <ncurses.h>	// includes stdio, unctrl, stdarg, stddef
			// TRUE, FALSE, OR, ERR
#include <stdlib.h>
#include <string.h>
#include "gap_buffer2.h"
#include "display.h"

#define NAME_SIZE 256

enum { PAIR_RW = 1, PAIR_BW, PAIR_WB, PAIR_BR, PAIR_BB};

typedef struct screen_pos_info	
{
	int c, x, y;

	int maxy, maxx;

	char name[NAME_SIZE];
	char mode;

	size_t before, after;

} screen_info;


char splash_screen();					// DONE : welcome screen when no file is provided 
void center_rowaddstr(int row, char *title);		// DONE	: places string centered along given row

void check_color();					// DONE : start color pairs
void check_resize();					// DONE	: Changes stdscr for size while waiting for proper resize
void check_gapbuffer(gap_buffer *gb);			// DONE : exit if gap_buffer dne
void check_window(WINDOW *win);				// DONE	: exit if window dne
void modeline(screen_info *info, gap_buffer *gb); 


void info_pos_init(screen_info *info);
void info_pos_init(screen_info *info)
{
	info->x = info->y = 0;
	info->mode = 'i';

	info->before = 0;
	info->after = (info->maxy - 2) * (info->maxx - 2);
}

char mode_select(screen_info *info, gap_buffer *gb);	// TODO
void insert_mode(screen_info *info, gap_buffer *gb);	// TODO
void normal_mode(screen_info *info, gap_buffer *gb);	// TODO

int main(int argc, char *argv[])
{	
	screen_info info;
	gap_buffer *gb;
	WINDOW *win;

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

	win = newwin(info.maxy - 2, info.maxx - 2, 1, 1);	// window check
	check_window(win);
	curs_set(1);

	bkgd(COLOR_PAIR(PAIR_BW));
	wbkgd(win, COLOR_PAIR(PAIR_RW));

	info_pos_init(&info);

	bool first = true;	
	while (1)
	{
		if (!first)
		{
			mode_select(&info, gb);
			modeline(&info, gb);
		}

		werase(win);
		waddnstr(win, gb->buffer, info.before);				// TODO : fix number of characters printed
		waddnstr(win, gb->endgap, info.after); 

		wmove(win, info.y, info.x);

		refresh();			// This refresh order works..
		wrefresh(win);
		first = false;
	}

	endwin();
	return 0;
}


char mode_select(screen_info *info, gap_buffer *gb)
{
	if (info->mode == 'i')
		insert_mode(info, gb);
	else
		info->mode = 'i';
//		normal_mode(info, gb);

	return info->c;
}


void insert_mode(screen_info *info, gap_buffer *gb)
{
	info->c = getch();

	char del;
	if (info->c == KEY_BACKSPACE)						// Deleting
	{
		if ((del = delete_c(gb)))
		{
			if (del == '\n')					// new line, calc diff to prev new line
			{
				info->x = (int) unti_new_line(gb, -1);
				info->y--;
			}

			else if (info->x > 0)					// std delete
				info->x--;

			info->before--;
		}
	}

	else if (info->c == '\n')
	{
		if (insert_c(gb, info->c))					// incriment y , x = 0 for newline
		{
			info->y++;
			info->x = 0;
			info->before++;
		}
	}
	else
	{
		if (insert_c(gb, info->c))					// insert case for any other character
		{
			info->x++;
			info->before++;
		}
	}
}

void modeline(screen_info *info, gap_buffer *gb) 
{
	move(LINES - 1, 1);
	clrtoeol();
	printw("mode : %s, ", (info->mode == 'n') ? "[normal]" : "[insert]");
	printw("X: %d, Y: %d, s: %zu, n: %s, (%c)", info->x, info->y, (gb->index + 1) - gb->gapsize, info->name, info->c);
}

char splash_screen() 
{
	curs_set(0);
	int c = '\0';
	int height, width;
	getmaxyx(stdscr, height, width);
	int pos = height / 4;

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

	while (1)
	{
		c = getch();
		
		if (c == 'c' || c == 'o' || c == 'n' || c == 'q')
                	break;

		mvaddch(pos, width / 2, c); 
		refresh();
	}

	curs_set(0);
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
