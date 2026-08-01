#include <ncurses.h>	// includes stdio, unctrl, stdarg, stddef, TRUE, FALSE, OR, ERR
#include <stdlib.h>
#include <string.h>
#include "gap_buffer2.h"
#include "display.h"

#define NAME_SIZE 256
#define MAX_FILELEN 4096

typedef struct rendered_screen {

	int available;
	int screen_size;
	int *highlights;
	char *screen;
	gap_buffer *gb;

} render;


bool init_hl_formats();					// DONE : start color pairs
render *init_screen(char *filename);
bool screen_populate(render *disp, size_t offset);
void errormsg(char *error);

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

	disp->available = LINES * COLS;
	disp->screen_size = 0;

	disp->highlights = malloc(sizeof(int) * (disp->available));
	if (!disp->highlights) return NULL;

	disp->screen = malloc(sizeof(char) * disp->available);
	if (!disp->screen) return NULL;

	return disp;
}


bool screen_populate(render *disp, size_t offset)
{
	char *buffer = disp->gb->buffer;
	char *gap = disp->gb->gap;
	size_t index = disp->gb->index;
	size_t before_gap = gap - buffer;
	char *screen = disp->screen;

	int lines = LINES;
	size_t bp = 0;
	int sp = 0;
	char c;
	bool gap_skip = true;
	bool off_skip = true;

	while (lines > 0 && sp < disp->available && bp < index)	// lines to add, index range of screen buffer, index range of gap_buffer
	{
		if (before_gap == 0 && gap_skip)	// skip gap space
		{
			bp += disp->gb->gapsize;
			gap_skip = false;
		}
		else 
			before_gap--;

		c = buffer[bp];
		if (c != '\0')
		{
			if (c == '\n' && offset != 0 && off_skip) 	// ignore lines until offset
				offset--;

			if (c == '\n' && offset == 0) 	// increment for each line
				lines--;

			if (offset == 0 && !off_skip) 		// any other character is added to screen buffer
				screen[sp++] = c;			
		}
		bp++;
		if (offset == 0)
			off_skip = false;
	
	}
	disp->screen[sp] = '\0';
	disp->screen_size = sp;

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


	size_t offset = 0;
	int y, x;

	while (offset != disp->gb->index - disp->available)
	{
	screen_populate(disp, offset);
	printw("%s", disp->screen);
	offset++;

	refresh();
	napms(50);
	clear();
	}
	printw("%s", disp->screen);
	refresh();
	getch();
	endwin();

	return 0;
}
