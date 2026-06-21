#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

size_t GAP_SIZE = 5;

typedef struct gb_structure {

	char *buffer;		// buffer + gap
	size_t index;		// buffer size - 1 for index
				//
	char *gap;		// ptr to gap
	size_t gapsize;		// size of gap
	char *endgap;		// ptr to end of gap

} gap_buffer;


gap_buffer *copyfiletobuffer(char *name); 	// DONE
gap_buffer *initgapbuffer(size_t size);		// DONE
void printbuffer(gap_buffer *gb);		// DONE

bool can_be_down_shift(gap_buffer *gb);		// DONE
bool can_be_up_shift(gap_buffer *gb);		// DONE

bool shift_down(gap_buffer *gb);		// DONE
bool shift_up(gap_buffer *gb);			// DONE

bool delete(gap_buffer *gb);			// DONE
bool insert(gap_buffer *gb);			// to-do

gap_buffer *initgapbuffer(size_t size)
{
	gap_buffer *gb;
	gb = malloc(sizeof(gap_buffer));
	if (!gb) return NULL;

	gb->buffer = calloc(size + GAP_SIZE, sizeof(char));
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

void printbuffer(gap_buffer *gb)
{
	char *p = gb->buffer;
	while (p <= gb->buffer + gb->index)
	{
		if (p == gb->gap)	
		{
			for (size_t i = 0; i < gb->gapsize; i++)
				putchar('_');
			p = gb->endgap;
		}
		else
		{
			putchar(*p);
			p++;
		}
	}
}


gap_buffer *copyfiletobuffer(char *name)
{
	FILE *fp = fopen(name, "r");
	if (!fp) 
		return NULL;

	if (fseek(fp, 0, SEEK_END))
	{
		fclose(fp);
		return NULL;
	}

	long size = ftell(fp);
	if (size < 0) 
	{
		fclose(fp);
		return NULL;
	}
	rewind(fp);


	gap_buffer *gb = initgapbuffer(size);

	int c;
	size_t pos = gb->gapsize;
	while (((c = getc(fp)) != EOF) && pos < gb->index)
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

bool delete(gap_buffer *gb)
{
	if (gb->gapsize == 0 || gb->gap == gb->buffer)	// nothing to delete or at start
		return false;

	gb->gap--;
	*gb->gap = '\0'; 

	gb->gapsize++;

	return true;
}

int main(int argc, char *argv[]) 
{
	gap_buffer *gb;

	if (argc == 2)
		gb = copyfiletobuffer(argv[1]);	// load file
	else
		gb = initgapbuffer(0);		// no file

	printf("%zu size\n", gb->index);

	printbuffer(gb);
	putchar('\n');
	printf("%zu size of gap\n", gb->gapsize);
	shift_down(gb);
	delete(gb);
	printf("%zu size of gap\n", gb->gapsize);
	shift_down(gb);
	shift_down(gb);
	printbuffer(gb);
	putchar('\n');

	return 0;
}
