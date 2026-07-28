#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#define GAP_SIZE 4096 // default gap size
typedef struct gb_structure {

	char *buffer;		
	size_t index;		// buffer size - 1 for index

	char *gap;			// begining / end of gap
	char *endgap;		
	size_t gapsize;		// size of gap
				
} gap_buffer;

gap_buffer *initgapbuffer(size_t size);	
gap_buffer *copyfiletobuffer(char *name); 		
void free_gap_buffer(gap_buffer *gb);
void printbuffer(gap_buffer *gb, bool visible);	

bool can_be_down_shift(gap_buffer *gb);		
bool can_be_up_shift(gap_buffer *gb);		

char shift_down(gap_buffer *gb);		
char shift_up(gap_buffer *gb);			

char delete_c(gap_buffer *gb);			
bool insert_c(gap_buffer *gb, char c);		
bool expandbuffer(gap_buffer *gb);		

size_t unti_new_line(gap_buffer *gb, int dir);

#endif
