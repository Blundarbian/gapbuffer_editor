#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#define GAP_SIZE 4096 

/* GAP_BUFFER visual with ptr locations
 * buffer[***********gap[00000000]endgap******]
 *
 * 			[--------] = gapsize
 * 	 [-----------------------------------] = index
 *
 * 	 gap ptr first index of the gap.
 * 	 end ptr first char after gap.
 */

typedef struct gb_structure {
			
	char *buffer;		
	size_t index;		// buffer size - 1 for index

	char *gap;		// beginning & end of gap ptr 
	char *endgap;		
	size_t gapsize;		// size of gap
				
} gap_buffer;

// GAP_BUFFER initializers, cleanup, and printing 
gap_buffer *initgapbuffer(size_t size);				
gap_buffer *copyfiletobuffer(char *filename); 		
void free_gap_buffer(gap_buffer *gb);
void printbuffer(gap_buffer *gb, bool visiblegap);	
bool safegapfile(gap_buffer *gb, char *filename);


// Shift up/down single char (left/right arrow movement)
bool can_be_down_shift(gap_buffer *gb);		
bool can_be_up_shift(gap_buffer *gb);		
char shift_down(gap_buffer *gb);		
char shift_up(gap_buffer *gb);			

// insert/delete single char
char delete_c(gap_buffer *gb);				
bool insert_c(gap_buffer *gb, char c);		

// expand buffer by GAP_SIZE once gap is exhausted 
bool expandbuffer(gap_buffer *gb);		

// give distance until newline or word
size_t until_new_line(gap_buffer *gb, int dir);	 
size_t until_new_word(gap_buffer *gb, int dir);	 

// newline shift (up/down arrow movement)
size_t shift_line(gap_buffer *gb, int dir, int xprev);

#endif
