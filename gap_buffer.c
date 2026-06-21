#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int GAP_SIZE = 10;

typedef struct gapbuffer{
	unsigned int bufsize;	// size of buffer
	unsigned int gapsize;	// size of gap
	char *gap;		// ptr to buffer and gap
	char *startgap;		// ptr start gap
	char *endgap;		// ptr end gap
	char *endofbuf;		// ptr end of entire buffer
} gb;


gb *initgapbuffer(int size);
void freegapbuffer(gb *buf);

void printbuffer(gb *buf);

void insertchar(gb *buf, char c);
void expandbuffer(gb *buf);

void deletechar(gb *buf, char c);

gb *initgapbuffer(int size) 
{
	gb *newgap = malloc(sizeof  *newgap);
	if (!newgap)
		return NULL;

	int len = size + GAP_SIZE;
	newgap->gap = calloc(len, sizeof(char));
	if (!newgap->gap)
	{
		free(newgap);
		return NULL;
	}
	
	newgap->bufsize = len;
	newgap->gapsize = GAP_SIZE;

	newgap->endgap= newgap->gap + size + GAP_SIZE - 1;
	newgap->startgap = newgap->gap + size - 1;
	newgap->endofbuf = newgap->gap + len - 1;

	return newgap;
}

void freegapbuffer(gb *buf)
{
	if (!buf) 
		return ;

	free(buf->gap);
	free(buf);
}

void printbuffer(gb *buf)
{
	unsigned int pos = 0;
	while (pos++ < buf->bufsize)
	{
		if (buf->gap[pos] == '\0')
			putchar('_');
		else 
			putchar(buf->gap[pos]);

	}
	putchar('\n');
}

void insertchar(gb *buf, char c) 
{
	if (buf->gapsize == 0)
		return;
		//expandbuffer(buf);

	*(buf->startgap++) = c;
	buf->gapsize--;
}

void deletechar(gb *buf, char c)
{

}

int main()
{
	gb *buf = initgapbuffer(10);

	for (int i = 0; i < 8; i++)
		buf->gap[i] = 'a' + i;
	buf->gap[9] = '\0';

	printbuffer(buf);

	freegapbuffer(buf);

	return 0;

}
