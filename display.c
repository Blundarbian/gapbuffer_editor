#include <ncurses.h>	// stdio, unctrl, stdarg, stddef, TRUE, FALSE, OR, ERR
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gap_buffer2.h"
#include "display.h"

#define MAX_FILELEN 4096

typedef struct rendered_screen {

	int available;
	int xprev;
	size_t offset;
	int *highlights;
	char *screen;
	gap_buffer *gb;

} render;


bool init_hl_formats();					
render *init_screen(char *filename);
void free_screen(render *disp);

bool screen_populate(render *disp);
void errormsg(char *error);
bool getinput(render *disp);

size_t getcursorline(render *disp);
void check_scroll(render *disp);


size_t getcursorline(render *disp)
{
	size_t line = 0;
	size_t pos = 0;
	gap_buffer *gb = disp->gb;

	while (pos < (size_t) (gb->gap - gb->buffer))
	{
		if (gb->buffer[pos] == '\n') 
			line++;

		pos++;
	}

	return line;
}


void check_scroll(render *disp)
{
	size_t cursor_line = getcursorline(disp);
	int vis = LINES - 1;

	if (cursor_line < disp->offset)			// scroll up 
		disp->offset = cursor_line;

	else if (cursor_line >= disp->offset + vis)	// scroll down one
		disp->offset = cursor_line - vis + 1;
}


render *init_screen(char *filename)
{
	render *disp = NULL;
	disp = malloc(sizeof(render));
	if (!disp) return NULL;

	if (strnlen(filename, MAX_FILELEN) == 0)	// new file
		disp->gb = initgapbuffer(0);
	else
	{
		disp->gb = copyfiletobuffer(filename);
		if (!(disp->gb)) disp->gb = initgapbuffer(0);	// file not found }
	}

	if (!disp->gb) return NULL;			// cannot be created

	disp->available = (LINES - 1)* COLS;
	disp->offset = 0;
	disp->xprev = 0;

	disp->highlights = malloc(sizeof(int) * (disp->available + 1));
	if (!disp->highlights) return NULL;

	disp->screen = malloc(sizeof(char) * (disp->available + 1));
	if (!disp->screen) return NULL;

	return disp;
}


void free_screen(render *disp) 
{
	if (!disp) return;
	free(disp->highlights);
	free(disp->screen);
	free_gap_buffer(disp->gb);
	free(disp);
}


bool screen_populate(render *disp)
{
	char *buffer = disp->gb->buffer;
	char *gap = disp->gb->gap;
	size_t index = disp->gb->index;
	size_t before_gap = gap - buffer;
	size_t offset = disp->offset;
	char *screen = disp->screen;

	int lines = LINES - 1;	// lines to display
	size_t bp = 0;		// buffer pos
	int sp = 0;		// screen pos
	char c;

	while (lines > 0 && sp < disp->available && bp <= index)	// lines to add, index range of screen buffer, index range of gap_buffer
	{

		if (bp == before_gap)	// skip gap space
		{
			screen[sp++] = '/';
			bp += disp->gb->gapsize;
			if (bp > index) break;
			continue;
		}

		c = buffer[bp];
		if (c != '\0')
		{
			if (offset > 0)
			{
				if (c == '\n') offset--;
			}
			else
			{
				if (c == '\n') lines--;
				screen[sp++] = c;
			}
		}
		bp++;
	}

	disp->screen[sp] = '\0';

	return true;
}


enum HL_FORMATS {
	HL_NORMALS,
	HL_COMMENT,
	HL_KEYWORD,
	HL_STRINGS,
	HL_LITERAL,
	HL_SEARCHS
};


bool init_hl_formats()
{
	if (!has_colors() || !can_change_color()) 
		return false;

	start_color();					
	init_pair(HL_NORMALS, COLOR_WHITE, COLOR_BLACK);	// name, text color, background
	init_pair(HL_COMMENT, COLOR_GREEN, COLOR_WHITE);
	init_pair(HL_KEYWORD, COLOR_BLACK, COLOR_BLACK);
	init_pair(HL_STRINGS, COLOR_BLACK, COLOR_BLACK);
	init_pair(HL_LITERAL, COLOR_WHITE, COLOR_BLACK);
	init_pair(HL_SEARCHS, COLOR_WHITE, COLOR_MAGENTA);

	return true;
}


void errormsg(char *error)
{
	printf("error: %s\n", error);
	exit(EXIT_FAILURE);
}

enum cval {
	backsapce = 8,
	ctrl_n = 14,
	ctrl_o = 15,
	ctrl_s = 19,
	ctrl_x = 24
};


bool getinput(render *disp)
{
	int c = getch();
	//int r;
	//size_t line;

	switch (c)
	{
		case ctrl_x:
			safegapfile(disp->gb, "test.txt");
			free_screen(disp);
			return false;

		case KEY_LEFT:
			shift_up(disp->gb);
			break;

		case KEY_RIGHT:
			shift_down(disp->gb);
			break;

		case KEY_UP:
			shift_line(disp->gb, -1, 2);
			break;

		case KEY_DOWN:
			shift_line(disp->gb, 1, 2);
			break;

		case KEY_BACKSPACE:
			delete_c(disp->gb);
			break;

		default: 
			insert_c(disp->gb, c);
			break;
	}
	check_scroll(disp);
	return true;
}


int main(int argc, char *argv[])
{	
	initscr();
	noecho();
	keypad(stdscr, TRUE);

	if (!init_hl_formats())
		errormsg("term does not support rgb");


	render *disp = init_screen(argv[1]);

	if (!disp)
		errormsg("cannot create gapbuffer");

	curs_set(0);
	while (1)
	{
		clear();
		screen_populate(disp);

		printw("%s", disp->screen);

		refresh();

		if (!getinput(disp))
			break;
	}
	endwin();

	return 0;
}
