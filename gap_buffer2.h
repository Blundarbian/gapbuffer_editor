#ifndef GAP_BUFFER_H
#define GAP_BUFFER_H

#define GAP_SIZE 4096 // default gap size
typedef struct gb_structure {

	char *buffer;		// buffer + gap
	size_t index;		// buffer size - 1 for index
				//
	char *gap;		// ptr to gap
	size_t gapsize;		// size of gap
	char *endgap;		// ptr to end of gap
				
} gap_buffer;

gap_buffer *copyfiletobuffer(char *name); 	
gap_buffer *initgapbuffer(size_t size);	
void free_gap_buffer(gap_buffer *gb);
void printbuffer(gap_buffer *gb, bool visible);	

bool can_be_down_shift(gap_buffer *gb);		
bool can_be_up_shift(gap_buffer *gb);		

char shift_down(gap_buffer *gb);		
char shift_up(gap_buffer *gb);			

char delete_c(gap_buffer *gb);			
bool insert_c(gap_buffer *gb, char c);		
bool expandbuffer(gap_buffer *gb);		

bool delete_word_up(gap_buffer *gb);
bool delete_word_down(gap_buffer *gb);
						
// Does not find words spanning across the gap... need to rewrite
bool move_findword(gap_buffer *gb, char *word);		// TODO
size_t move_nextword_up(gap_buffer *gb);	
size_t move_nextword_down(gap_buffer *gb);
					
size_t unti_new_line(gap_buffer *gb, int dir);

#endif
