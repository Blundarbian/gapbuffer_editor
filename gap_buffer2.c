#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "gap_buffer2.h"


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


bool safegapfile(gap_buffer *gb, char *name)
{
	if (!gb) return false;

	FILE *fp = fopen(name, "w");
	if (!fp) return false;

	size_t cur = 0;
	size_t gindex = gb->gap - gb->buffer;
	size_t eindex = gindex + gb->gapsize;

	while (cur < gindex)
	{
		putc(gb->buffer[cur], fp);
		cur++;
	}
	
	cur = eindex;
	while (cur < gb->index)
	{
		putc(gb->buffer[cur], fp);
		cur++;
	}
	fclose(fp);
	
	return true;
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

	size_t size = ftell(fp);		// file length ftell check
	if (size == 0) 
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


char shift_down(gap_buffer *gb)
{
	if (!can_be_down_shift(gb))
		return '\0';

	char eg = *gb->endgap;
	*gb->gap = *gb->endgap; 
	gb->gap++;             
	gb->endgap++;         

	return eg;
}


char shift_up(gap_buffer *gb)
{
	if (!can_be_up_shift(gb))
		return '\0';

	char eg = *gb->endgap;
	gb->endgap--;
	gb->gap--;
	*gb->endgap = *gb->gap;
	
	return eg;
}


char delete_c(gap_buffer *gb)
{
	if (gb->gap == gb->buffer)	// at start
		return '\0';

	gb->gap--;
	char del = *gb->gap;
	*gb->gap = '\0'; 

	gb->gapsize++;

	return del;
}


bool insert_c(gap_buffer *gb, char c)
{
	// request gb expansion if all positions in gap are filled
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
	size_t offset = gb->gap - gb->buffer;	
	size_t endset = gb->endgap - gb->buffer;
	size_t oldsize = gb->index + 1;		

	char *nbuf = realloc(gb->buffer, oldsize + GAP_SIZE);
	if (!nbuf) return false;

	gb->buffer = nbuf;			
	gb->gap = gb->buffer + offset;		// gap set back to start
	gb->endgap = gb->buffer + endset;	// endgap set back

	size_t tail = oldsize - endset;		// tail of chars after realloc

	memmove(gb->endgap + GAP_SIZE, gb->endgap, tail);
	
	gb->gapsize += GAP_SIZE;		
	gb->index += GAP_SIZE;
	gb->endgap += GAP_SIZE;

	return true;
}


size_t until_new_line(gap_buffer *gb, int dir)
{
	size_t dist= 0;
	char *p = gb->gap;
	char *e = gb->endgap - 1;
	char c;

	if (dir < 0)	
	{
		while (p > gb->buffer)
		{
			c = *--p;

			if (c == '\n')
				break;

			dist++;
		}
		return dist;
	}
	else
	{
		while (e < (gb->buffer + gb->index))
		{
			c = *++e;

			if (c == '\n')
				break;

			dist++;
		}
		return dist;
	}
}


size_t until_new_word(gap_buffer *gb, int dir)
{
	size_t dist= 0;
	char *p = gb->gap;
	char *e = gb->endgap - 1;
	char c;

	if (dir < 0)
	{
		if ((c = *--p) == ' ')
			while (p > gb->buffer && c == ' ')
			{
				c = *--p;
				dist++;
			}
		else
			while (p > gb->buffer && c != ' ')
			{
				c = *--p;
				dist++;
			}
	}
	else
	{
		if ((c = *++e) == ' ')
			while (e < (gb->buffer + gb->index) && c == ' ')
			{
				c = *++e;
				dist++;
			}
		else
			while (e < (gb->buffer + gb->index) && c != ' ')
			{
				c = *++e;
				dist++;
			}
	}
	return dist;
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
				delete_c(gb);
				break;
			case '4' :
				printf("%zu\n", until_new_line(gb, 1));
				break;
			case '5' :
				printf("%zu\n", until_new_word(gb, 1));
				break;
			case '6' :
				printf("%d\n", safegapfile(gb, "savetest.txt"));
				break;
			/*case 'b' :
				move_nextword_up(gb);
				break;
			case 'f' :
				move_nextword_down(gb);
				break;
			case 's' :
				move_findword(gb, "Five");
				break;
			*/
			default:
				if (c != '\n')
					insert_c(gb, c);
				break;
		}
	}

	free_gap_buffer(gb);
	return 0;
}

