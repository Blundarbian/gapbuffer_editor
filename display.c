#include <ncurses.h>	// stdio, unctrl, stdarg, stddef, TRUE, FALSE, OR, ERR
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "gap_buffer2.h"
#include "display.h"

#define MAX_FILELEN 4096

typedef struct rendered_screen {

	size_t offset;
	int available, xprev, line_num;
	int *highlights;
	char *filename;
	char *screen;
	gap_buffer *gb;

} render;


bool init_hl_formats();					
render *init_screen(char *filename);
void free_screen(render *disp);

bool screen_populate(render *disp);
void render_screen_modeline(render *disp, WINDOW *modeline);
void errormsg(char *error, render *disp);
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

	int len = strnlen(filename, MAX_FILELEN);	// new file
	if (len == 0)
		disp->gb = initgapbuffer(0);
	else
	{
		disp->gb = copyfiletobuffer(filename);
		if (!(disp->gb)) disp->gb = initgapbuffer(0);	// file not found }
	}

	if (!disp->gb) return NULL;			// cannot be created

	disp->available = (LINES - 1) * COLS;
	disp->offset = disp->xprev = disp->line_num = 0;

	disp->filename = malloc(sizeof(char) * len);
	if (len == 0)	strcpy(disp->filename, "new-file");
	else		strcpy(disp->filename, filename);


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
	HL_CONTROL,
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
	init_pair(HL_CONTROL, COLOR_BLACK, COLOR_WHITE);
	init_pair(HL_COMMENT, COLOR_GREEN, COLOR_BLACK);
	init_pair(HL_KEYWORD, COLOR_BLACK, COLOR_BLACK);
	init_pair(HL_STRINGS, COLOR_BLACK, COLOR_BLACK);
	init_pair(HL_LITERAL, COLOR_WHITE, COLOR_BLACK);
	init_pair(HL_SEARCHS, COLOR_WHITE, COLOR_MAGENTA);

	return true;
}


void errormsg(char *error, render *disp)
{
	free_screen(disp);
	endwin();
	printf("error: %s\n", error);
	exit(EXIT_FAILURE);
}

enum cval {

	CTRL_H = 8,
	CTRL_N = 14,
	CTRL_O = 15,
	CTRL_S = 19,
	CTRL_W = 23,
	CTRL_X = 24
};

void render_screen_modeline(render *disp, WINDOW *modeline)
{
	mvprintw(0, 0, "%s", disp->screen);

	wclear(modeline);
	wprintw(modeline, "file: %s, line: %d, ^X quit/save ^H help", disp->filename, disp->line_num);

	refresh();
	wrefresh(modeline);
}

bool getinput(render *disp)
{
	int c = getch();
	gap_buffer *gb = disp->gb;

	switch (c)
	{
		case CTRL_X:
			safegapfile(gb, "test.txt");
			free_screen(disp);
			return false;

		case KEY_LEFT:
			shift_up(gb);
			disp->xprev = until_new_line(gb, -1);
			break;

		case KEY_RIGHT:
			shift_down(gb);
			disp->xprev = until_new_line(gb, -1);
			break;

		case KEY_UP:
			if (shift_line(gb, -1, disp->xprev))
				disp->line_num--;

			break;

		case KEY_DOWN:
			if (shift_line(gb, 1, disp->xprev))
				disp->line_num++;
			break;

		case KEY_BACKSPACE:
			if (delete_c(gb) != '\0')
				disp->xprev--;
			break;

		default: 
			if (insert_c(gb, c))
				disp->xprev++;
			
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

	WINDOW *modeline = newwin(0, COLS, LINES-1, 0);
	render *disp = NULL;

	if (!init_hl_formats())	errormsg("term does not support rgb", disp);
	if (!modeline) 		errormsg("cannot create modeline", disp);


	if (argc < 2)		disp = init_screen("");
	else if (argc == 2)	disp = init_screen(argv[1]);

	else			errormsg("./display 'filename'", disp);
	if (!disp) 		errormsg("cannot create gapbuffer", disp);


	curs_set(0);
	bkgd(COLOR_PAIR(HL_NORMALS));
	wbkgd(modeline, COLOR_PAIR(HL_CONTROL));
	while (1)
	{
		clear();
		screen_populate(disp);

		render_screen_modeline(disp, modeline);
		
		if (!getinput(disp))
			break;
	}
	endwin();

	return 0;
}
