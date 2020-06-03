#include "theory.h"
#include "visualization.h"
#include "clear_screen.h"
#include <iostream>
#include <fstream>
#include <limits>
#include <locale>
#include <windows.h>
#include <pdcurses/curses.h>

#define MIDX getmaxx(stdscr)/2
#define MIDY getmaxy(stdscr)/2

using namespace std;
using namespace DL;

#define dock_middle(y,s) mvaddstr(y,MIDX - strlen(s)/2,s);

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
        case 'q':
            exit(0);
        break;
        default:
            break;
        }
        clear();
        box(stdscr,0,0);
        wborder(stdscr, '|', '|', '-', '-', '+', '+', '+', '+');
        

        dock_middle(MIDY + 1,"������� ������� ������ ��� ����, ����� ����� ������� ������, ������� q ����� �����, ��� ������� ������� ������ ����� ������� ������ �����");
        mvprintw(MIDY + 2,2,"�������� ������");
        mvprintw(MIDY + 3,2,"������ ����");
        mvprintw(MIDY + 4,2,"���������� ������������");
        mvaddch(MIDY + current_option % 3 + 2,1,'>');

        dock_middle(0,"� ���������");
        dock_middle(2,"�������� ������");
        dock_middle(4,"�� ���������� \"���������������� �  �����������\"");
        dock_middle(6,"������-���������������� �������� \"���������� ������\"");
        dock_middle(MIDY - 3,"�������� ������� ������ �����-11 ������� ������");
        dock_middle(MIDY - 1,"���� 2020");

        mvhline(MIDY,1,'-',getmaxx(stdscr) - 2);
        mvaddstr(MIDY,MIDX - 6,"������� ����");

        refresh();
        choise = getch();
	}
}
