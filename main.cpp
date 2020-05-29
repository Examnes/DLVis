#include "theory.h"
#include "visualization.h"
#include "clear_screen.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <locale>
#include <windows.h>
#include <pdcurses/curses.h>

using namespace std;
using namespace DL;

template<typename T>
T read(bool(*predicate)(T,T),T arg = 0)
{
    T x = 0;
    cin >> x;
    while((cin.fail())|| !predicate(x,arg))
    {
        cin.clear();
        cin.ignore(std::numeric_limits<streamsize>::max(),'\n');
        cin >> x;
    }
    return x;
}

template<typename T>
bool all(T a,T b)
{
  return true; 
}

int main(int, char**) 
{
    system("chcp 1251");
    system("mode con:cols=240 lines=250");
    theory teo = theory("theory.bin","questions.bin");
    visualization vi = visualization();
    initscr();
    setvbuf(stdout, NULL, _IOLBF, 0);
    cbreak();
    keypad(stdscr, TRUE);
    noecho();
    refresh();

    short current_option = 255;
    mvaddstr(0,0,"нажмите стрелку ввверх или вниз, чтобы чтобы листать пункты, нажмите q чтобы выйти, или нажмите стрелку вправо чтобы выбрать данный пункт");
    mvprintw(0,1," Почитать теорию\n Пройти тест\n Посмотреть визуализацию\n");
    mvaddch(current_option % 3 + 2,0,'>');
    refresh();
    int choise = 0;
	while(choise != 'q')
	{
        switch (choise)
        {
        case KEY_UP:
            current_option = (current_option - 1);
            break;
        case KEY_DOWN:
            current_option = (current_option + 1);
            break;
        case KEY_RIGHT:
            switch (current_option % 3)
            {
            case 0:
                teo.start_read();
                break;
            case 1:
                teo.start_test();
                break;
            case 2:
                vi.visualization_start();
                break;
            }
            break;
        default:
            break;
        }
        clear();
        mvaddstr(0,0,"нажмите стрелку ввверх или вниз, чтобы чтобы листать пункты, нажмите q чтобы выйти, или нажмите стрелку вправо чтобы выбрать данный пункт");
        mvprintw(1,0," Почитать теорию\n Пройти тест\n Посмотреть визуализацию\n");
        mvaddch(current_option % 3 + 1,0,'>');
        refresh();
        choise = getch();
	}
}
