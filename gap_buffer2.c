#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "gap_buffer2.h"

#define GAP_SIZE 4096		// default gap size


gap_buffer *initgapbuffer(size_t size)
{
	gap_buffer *gb;
	gb = malloc(sizeof(gap_buffer));				// alloc for gap_buffer struct
	if (!gb) return NULL;

	gb->buffer = calloc(size + GAP_SIZE, sizeof(char));		// alloc for buffer array
	if (!(gb->buffer)) 
	{
		free(gb);
		return NULL;
	}

	gb->index = GAP_SIZE + size - 1;// gap + file size -1	

	gb->gap = gb->buffer;		// gap ptr is first index start
	gb->gapsize = GAP_SIZE;		// gap starts at gap size
	gb->endgap = gb->buffer + GAP_SIZE; // gap ptr end

	return gb;
}


void printbuffer(gap_buffer *gb, bool visible)
{
	char *p = gb->buffer;
	while (p <= gb->buffer + gb->index)
	{
		if (p == gb->gap)	
		{
			if (visible)
			{
				for (size_t i = 0; i < gb->gapsize; i++)
					putchar('_');
			}
			else 
				putchar('|');

			p = gb->endgap;
		}
		else
		{
			putchar(*p);
			p++;
		}
	}
}


void free_gap_buffer(gap_buffer *gb)
{
	if (!gb) 
		return;	

	if (!(gb->buffer))	
	{
		free(gb);
		gb = NULL;
		return;
	}

	free(gb->buffer);
	gb->buffer = NULL;
	gb->endgap = NULL;
	gb->gap = NULL;
}


gap_buffer *copyfiletobuffer(char *name)
{
	FILE *fp = fopen(name, "r");		
	if (!fp) 
		return NULL;

	if (fseek(fp, 0, SEEK_END))	// fseek check 
	{
		fclose(fp);
		return NULL;
	}

	long size = ftell(fp);		// file length ftell check
	if (size < 0) 
	{
		fclose(fp);
		return NULL;
	}
	rewind(fp);			// rewind from EOF to begin copying input


	gap_buffer *gb = initgapbuffer(size);		// initilize gap_buffer based off file size

	int c;
	size_t pos = gb->gapsize;				// start copying after gap
	while (((c = getc(fp)) != EOF) && pos <= gb->index)	// gap is in initilized in the front of the buffer
	{
		gb->buffer[pos] = c;
		pos++;
	}
	fclose(fp);

	return gb;
}


bool can_be_up_shift(gap_buffer *gb)
{
	if (gb->gap > gb->buffer)
		return true;
	return false;
}


bool can_be_down_shift(gap_buffer *gb)
{	
	if (gb->endgap < (gb->buffer + gb->index))
		return true;
	return false;
}


bool shift_down(gap_buffer *gb)
{
	if (!can_be_down_shift(gb))
		return false;

	*gb->gap = *gb->endgap; // gap (empty) = end of gap 
	gb->gap++;              // gap shifts to empty spot 
	gb->endgap++;          	// endgap shifts aswell

	return true;
}


bool shift_up(gap_buffer *gb)
{
	if (!can_be_up_shift(gb))
		return false;

	gb->endgap--;
	gb->gap--;
	*gb->endgap = *gb->gap;
	
	return true;
}


char delete(gap_buffer *gb)
{
	if (gb->gap == gb->buffer)	// at start
		return '\0';

	gb->gap--;
	char del = *gb->gap;
	*gb->gap = '\0'; 

	gb->gapsize++;

	return del;
}


bool insert(gap_buffer *gb, char c)
{
	if (gb->gapsize == 1) {
		if (!(expandbuffer(gb)))
			return false;
	}

	*gb->gap = c;
	gb->gap++;
	gb->gapsize--;
	
	return true;
}


bool expandbuffer(gap_buffer *gb) 
{
	size_t offset = gb->gap - gb->buffer;	// buf to gap start
	size_t endset = gb->endgap - gb->buffer;// buf to end gap stop

	size_t oldsize = gb->index + 1;		// old size
	size_t newsize = oldsize + GAP_SIZE;	// new size

	char *nbuf = realloc(gb->buffer, newsize);
	if (!nbuf) return false;

	gb->buffer = nbuf;			
	gb->gap = gb->buffer + offset;		// gap set back to start
	gb->endgap = gb->buffer + endset;	// endgap set back

	size_t tail = oldsize - endset;		// tail of chars after realloc
	memmove(gb->endgap + GAP_SIZE, gb->endgap, tail);
	// move tail '\0' from realloc to positions to 'fuse' into engap
	// idk if memmove is the fastest function to do this...
	//
	/* buf->[_______gap[0]end____]		out of space
	 * buf->[_______gap[0]end____0000000]	realloc more space
	 * buf->[_______gap[0000000]end____] 	move newspace to be used by pointers
	 */
	
	gb->gapsize += GAP_SIZE;		// incriment counters
	gb->index += GAP_SIZE;
	gb->endgap += GAP_SIZE;

	return true;
}


bool move_findword(gap_buffer *gb, char *word)		// TODO
{
	size_t dex; 
	size_t cursor = gb->gap - gb->buffer;

	return false;
}

bool delete_word_down(gap_buffer *gb)			// TODO
{

}

bool delete_word_up(gap_buffer *gb)			// TODO
{
	long corsor = (gb->gap - gb->buffer) - 1;
	size_t shift = 0;
	
	if ((shift = move_nextword_up(gb) == 0))
		return false;		

}


size_t move_nextword_up(gap_buffer *gb)	
{
	long cursor = (gb->gap - gb->buffer) - 1;
	size_t pos = 0;

	if (gb->buffer[cursor] == ' ') 
	{
		while (gb->buffer[cursor] == ' ' && cursor >= 0) 
		{
			shift_up(gb);
			pos++;
			cursor--;
		}
		return pos;
	}
	
	while (gb->buffer[cursor] != ' ' && cursor >= 0)
	{
		shift_up(gb);
		pos++;
		cursor--;
	}
	return pos;
}


size_t move_nextword_down(gap_buffer *gb)
{
	size_t cursor = gb->endgap - gb->buffer;
	size_t pos = 0;

	if (gb->buffer[cursor] == ' ') 
	{
		while (gb->buffer[cursor] == ' ' && cursor < gb->index) 
		{
			shift_down(gb);
			pos++;
			cursor++;
		}
		return pos;
	}
	
	while (gb->buffer[cursor] != ' ' && cursor < gb->index)
	{
		shift_down(gb);
		pos++;
		cursor++;
	}
	return pos;
}


int main(int argc, char *argv[]) 
{
	gap_buffer *gb = NULL;

	if (argc == 2)
		gb = copyfiletobuffer(argv[1]);	// load file
	else
		gb = initgapbuffer(0);		// no file
	
	if (!gb) 
		return -1;

	int c;
	while ((c = getchar()) != EOF)
	{
		printbuffer(gb, false);
		switch (c)
		{
			case '1' :
				shift_up(gb);
				break;
			case '2' :
				shift_down(gb);
				break;
			case '3' :
				delete(gb);
				break;
			case 'b' :
				move_nextword_up(gb);
				break;
			case 'f' :
				move_nextword_down(gb);
				break;
			case 's' :
				move_findword(gb, "Five");
				break;
			default:
				if (c != '\n')
					insert(gb, c);
				break;
		}
	}

	free_gap_buffer(gb);
	return 0;
}

