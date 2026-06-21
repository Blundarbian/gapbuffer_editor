#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

size_t GAP_SIZE = 5;

typedef struct gb_structure {

	char *buffer;		// buffer + gap
	size_t bsize;		// buffer size
				//
	char *gap;		// ptr to gap
	size_t gapsize;		// size of gap
	size_t startpos;	// position of start gap
	size_t endpos;		// position of end gap
	char *endgap;		// ptr to end of gap

	char *endbuffer;	// ptr to end of buffer

} gap_buffer;


gap_buffer *copyfiletobuffer(char *name); 	// DONE
gap_buffer *initgapbuffer(size_t size);		// DONE
void printbuffer(gap_buffer *gb);		// DONE


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

	gb->bsize = GAP_SIZE + size;	// gap + file size
	gb->gap = gb->buffer;		// gap ptr is first index start
	gb->gapsize = GAP_SIZE;		// gap starts at gap size
	gb->startpos = 0;		// first index is gap
	gb->endpos = GAP_SIZE - 1;		// gap last index
	gb->endgap = gb->buffer + GAP_SIZE - 1; // gap ptr end
	gb->endbuffer = gb->buffer + GAP_SIZE + size - 1;

	return gb;
}

void printbuffer(gap_buffer *gb)
{
	size_t pos = 0; 
	while (pos < gb->bsize)
	{
		if (gb->buffer[pos] == '\0')
			putchar('_');
		else
			putchar(gb->buffer[pos]);

		pos++;
	}
}


gap_buffer *copyfiletobuffer(char *name)
{
	FILE *fp = fopen(name, "r+");
	if (!fp) 
		return NULL;

	if (fseek(fp, 0, SEEK_END))
	{
		fclose(fp);
		return NULL;
	}

	size_t size = ftell(fp);
	if (size < 0) 
	{
		fclose(fp);
		return NULL;
	}
	rewind(fp);


	gap_buffer *gb = initgapbuffer(size);

	int c;
	size_t pos = gb->endpos;
	while (((c = getc(fp)) != EOF) && pos < gb->bsize)
	{
		gb->buffer[pos] = c;
		pos++;
	}
	fclose(fp);

	return gb;
}



int main(int argc, char *argv[]) 
{
	gap_buffer *gb = copyfiletobuffer(argv[1]);

	printf("%zu size\n", gb->bsize);
	printf("%c first char\n", *(gb->endgap++));
	printbuffer(gb);

	return 0;
}
