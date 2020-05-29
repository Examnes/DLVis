#include "utils.h"
#include <sstream>

std::string save_window(WINDOW* w)
{
    std::stringstream save;
    int width, height;
    getmaxyx(w, width, height);
    for(int i = 0;i < width;i++)
        for(int j = 0;j < height;j++)
            {
                char c = mvwinch(w,i,j);
                save.put(c);
            }
    return save.str();
}

WINDOW *create_newwin(int height, int width, int starty, int startx)
{
    WINDOW *local_win;
    local_win = newwin(height, width, starty, startx);
    box(local_win, 0, 0);
    wborder(local_win, '|', '|', '-', '-', '+', '+', '+', '+');
    wrefresh(local_win);
    WINDOW * derwin_self = derwin(local_win, height - 2, width - 2, 1, 1);
    return derwin_self;
}

void mvwrect(WINDOW *&w, int y, int x, int width, int height)
{
    mvwhline(w, y, x, '-', width);
    mvwhline(w, y + height - 1, x, '-', width);
    mvwvline(w, y, x + width, '|', height - 1);
    mvwvline(w, y, x, '|', height - 1);
    mvwaddch(w, y, x, '+');
    mvwaddch(w, y + height - 1, x, '+');
    mvwaddch(w, y, x + width, '+');
    mvwaddch(w, y + height - 1, x + width, '+');
    wrefresh(w);
}

void mvwclrrect(WINDOW *w, int y, int x, int width, int height)
{
    for (int i = 0; i < width; i++)
        for (int j = 0; j < height; j++)
            mvwaddch(w, y + j, x + i, ' ');
    wrefresh(w);
}
