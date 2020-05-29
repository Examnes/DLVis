#if !defined(PDUTILS_H)
#define PDUTILS_H
#include "pdcurses/curses.h"
#include <string>

std::string save_window(WINDOW* w);
WINDOW* create_newwin(int height, int width, int starty, int startx);
void mvwrect(WINDOW *&w, int y, int x, int width, int height);
void mvwclrrect(WINDOW *w, int y, int x, int width, int height);

#endif // PDUTILS_H
