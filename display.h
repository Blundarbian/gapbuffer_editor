#ifndef DISPLAY_H
#define DISPLAY_H
enum { PAIR_ERR = 0, PAIR_BW, PAIR_WB, PAIR_BR, PAIR_BB};

char splash_screen();
void resize_check();	
void center_rowaddstr(int row, char *title);

#endif
