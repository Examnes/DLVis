#include "theory.h"
#include "clear_screen.h"

#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>
#include <sstream>
#include <WinUser.h>
#include <algorithm>

#include <pdcurses/curses.h>

namespace DL
{

#define GETBIT(x, k) ((x & ( 1 << k )) >> k)
#define SETBIT(x, k) x = x | 1 << k
#define UNSETBIT(x, k) x = x & ( ~(1 << k) ); 

void read_theory_file(std::stringstream theory_file, std::string* pages, uint16_t& page_count)
{
    while (!theory_file.eof())
    {
        std::string current_page; 
        std::string line;
        do
        {
            getline(theory_file,line);
        } while (!line.length());

        do
        {
            current_page+=line + "\n";
            do
            {
                getline(theory_file,line);
            } while (!line.length());
        } while (line != "+++\r" && !theory_file.eof());
        pages[page_count++] = current_page;
    }
    
}

void read_question_file(std::stringstream question_file, std::string* questions, uint16_t& question_count,uint8_t* answers,uint16_t* options_count,std::string options[][8])
{
    while (!question_file.eof())
    {
        std::string question;
        do
        {
            getline(question_file,question);
        } while (std::all_of(question.begin(),question.end(),isspace));
        questions[question_count] = question;
        question_file >> options_count[question_count];
        for(uint16_t i = 0; i < options_count[question_count]; i++)
            do
            {
                getline(question_file,options[question_count][i]);
            } while (std::all_of(options[question_count][i].begin(),options[question_count][i].end(),isspace));
        uint16_t right_answer;
        do
        {
            getline(question_file,question);
        } while (std::all_of(question.begin(),question.end(),isspace));
        right_answer = std::stol(question);
        answers[question_count] = (uint8_t)right_answer;
        question_count++;
    }
}

std::stringstream decryptFile(std::string filename,char key = 42)
{
    std::ifstream file(filename, std::ios::binary);
    std::stringstream result;
    while(!file.eof())
    {
        char temp;
        file.read((char*)&temp,1);
        temp = temp ^ key;
        result.write(&temp,1);
    }
    file.close();
    return result;
}

theory::theory(std::string theorybase, std::string questionbase)
{
    current_page = 128;
    page_count = 0;
    question_count = 0;
    memset(answers, 0, sizeof(answers));
    memset(options_count, 0, sizeof(options_count));
    memset(solved, 0, sizeof(solved));
    read_theory_file( decryptFile(theorybase),pages,page_count);
    read_question_file( decryptFile(questionbase),questions,question_count,answers,options_count,options);
}

void theory::print_question(uint8_t question,uint8_t choose,uint8_t answer)
{
    mvprintw(5,0,"Вопрос: %s",questions[question].c_str());
    int offset = getcury(stdscr);
    mvaddstr(offset + 1,0,"Варианты ответа:");
    mvaddch(offset + 2 + choose,0,'>');
    for(uint8_t i = 0; i < options_count[question]; i++)
    {
        if(GETBIT(answer,i)) std::cout << "[выбрано]";
        mvprintw(offset + 2 + i,1,"%s%i. %s",GETBIT(answer,i) ? "[выбрано]" : "",i + 1,options[question][i].c_str());
    }
    refresh();
}

void theory::start_read()
{
    mvaddstr(0,0,pages[current_page % page_count].c_str());
    mvaddstr(getmaxy(stdscr) - 2,0,"Нажмите стрелку влево, чтобы листать влево и стрелку впрво, чтобы листать вправо или нажмите q, чтобы выйти");
    mvprintw(getmaxy(stdscr) - 1,0,"Вы на странице: %i",current_page % page_count);
    refresh();
    int choise = 0;
	do
	{
        if(choise == KEY_LEFT)
        {
            current_page = (current_page - 1);
        }
        else if(choise == KEY_RIGHT)
        {
            current_page = (current_page + 1);
        }
        else if(choise == 'q')
        {
            break;
        }
        clear();
        mvaddstr(0,0,pages[current_page % page_count].c_str());
        mvaddstr(getmaxy(stdscr) - 2,0,"Нажмите стрелку влево, чтобы листать влево и стрелку впрво, чтобы листать вправо или нажмите q, чтобы выйти");
        mvprintw(getmaxy(stdscr) - 1,0,"Вы на странице: %i",current_page % page_count);
        refresh();
        choise = getch();
	}while(choise != 'q');
    clear();
    initscr();
}

void theory::start_test()
{
    uint8_t choose = 0;
    uint8_t question = 0;
    uint8_t solved_count = 0;
    uint8_t total_passed = 1;
    uint8_t user_answer = 0;
    do
    {
        question = rand() % question_count;
    } while (solved[question]);

    mvaddstr(0,0,"Нажимайте стрелку вниз или стрелку вверх, чтобы листать варианты ответа.");
    mvaddstr(1,0,"Нажимите стрелку вправо, чтобы выбрать вариант ответа");
    mvaddstr(2,0,"Нажмите enter, чтобы отправить ответ или стрелку влево, чтобы пропустить вопрос и вернуться к нему позже.");
    mvaddstr(3,0,"Или нажмите q чтобы досрочно выйти из теста.");
    refresh();
    int choise = 0;
	do
	{
        clear();
        mvaddstr(0,0,"Нажимайте стрелку вниз или стрелку вверх, чтобы листать варианты ответа.");
        mvaddstr(1,0,"Нажимите стрелку вправо, чтобы выбрать вариант ответа");
        mvaddstr(2,0,"Нажмите enter, чтобы отправить ответ или стрелку влево, чтобы пропустить вопрос и вернуться к нему позже.");
        mvaddstr(3,0,"Или нажмите q чтобы досрочно выйти из теста.");
        switch (choise)
        {
        case KEY_LEFT:
            do
            {
                question = rand() % question_count;
            } while (solved[question]);
            choose = 0;
            break;
        case KEY_RIGHT:
            if(!GETBIT(user_answer,choose))
            {

                SETBIT(user_answer,choose);
            }else 
            {
                UNSETBIT(user_answer,choose);
            }
            break;
        case KEY_UP:
            choose = (choose - 1) % options_count[question];
            break;
        case KEY_DOWN:
            choose = (choose + 1) % options_count[question];
            break;
        case '\n':
            if( (answers[question] - user_answer) == 0)
            {
                solved_count++;
            }
            solved[question] = true;
            total_passed++;
            do
            {
                question = rand() % question_count;
            } while (solved[question]);
            choose = 0;
            user_answer = 0;
            break;
        case 'q':
            total_passed = 100;
            break;
        default:
            break;
        }
        print_question(question,choose,user_answer);
        choise = getch();
	}while(total_passed < question_count);
    clear();
    if(solved_count < 5)
    {
        mvaddstr(0,0,"Вы не прошли тест");
    } else  mvaddstr(0,0,"Вы прошли тест");
    refresh();
    getch();
    initscr();
}

theory::~theory()
{

}

} // namespace DL
