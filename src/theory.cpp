#include "theory.h"
#include "clear_screen.h"

#include <iostream>
#include <string>
#include <fstream>
#include <assert.h>
#include <sstream>
#include <WinUser.h>
#include <algorithm>
#include <chrono>

#include <pdcurses/curses.h>

namespace DL
{

#define GETBIT(x, k) ((x & (1 << k)) >> k)
#define SETBIT(x, k) x = x | 1 << k
#define UNSETBIT(x, k) x = x & (~(1 << k));

    void read_theory_file(std::stringstream theory_file, std::string *pages, uint16_t &page_count)
    {
        while (!theory_file.eof())
        {
            std::string current_page;
            std::string line;
            do
            {
                getline(theory_file, line);
            } while (!line.length());

            do
            {
                current_page += line + "\n";
                do
                {
                    getline(theory_file, line);
                } while (!line.length());
            } while (line != "+++\r" && !theory_file.eof());
            pages[page_count++] = current_page;
        }
    }

    void read_question_file(std::stringstream question_file, std::string *questions, uint16_t &question_count, uint8_t *answers, uint16_t *options_count, std::string options[][8])
    {
        while (!question_file.eof())
        {
            std::string question;
            std::getline(question_file,question,'\n');
            questions[question_count] = question;
            std::string cost;
            std::getline(question_file,cost,'\n');
            cost[question_count] = std::atoi(cost.c_str());
            std::string option;
            do
            {
                std::getline(question_file,option,'\n');
                if(option != "###\r")
                {
                    options[question_count][options_count[question_count]++] = option;
                }
            } while (option != "###\r" && !question_file.eof());
            uint8_t answer = 0;
            int mul = 1;
            for(int i = 0;i < options_count[question_count];i++)
            {
                if(options[question_count][i][0] == '+')
                {
                    answer += mul;
                    options[question_count][i].erase(options[question_count][i].begin());
                }
                mul *= 2;
            }
            answers[question_count] = answer;
            question_count++;
        }
    }

    std::stringstream decryptFile(std::string filename, char key = 42)
    {
        std::ifstream file(filename, std::ios::binary);
        std::stringstream result;
        while (!file.eof())
        {
            char temp;
            file.read((char *)&temp, 1);
            temp = temp ^ key;
            result.write(&temp, 1);
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
        read_theory_file(decryptFile(theorybase), pages, page_count);
        read_question_file(decryptFile(questionbase), questions, question_count, answers, options_count, options);
    }

    void theory::print_question(uint8_t question, uint8_t choose, uint8_t answer)
    {
        mvprintw(5, 0, "Вопрос: %s", questions[question].c_str());
        int offset = getcury(stdscr);
        mvaddstr(offset + 1, 0, "Варианты ответа:");
        mvaddch(offset + 2 + choose, 0, '>');
        for (uint8_t i = 0; i < options_count[question]; i++)
        {
            if (GETBIT(answer, i))
                std::cout << "[выбрано]";
            mvprintw(offset + 2 + i, 1, "%s%i. %s", GETBIT(answer, i) ? "[выбрано]" : "", i + 1, options[question][i].c_str());
        }
        refresh();
    }

    void theory::start_read()
    {
        mvaddstr(0, 0, pages[current_page % page_count].c_str());
        mvaddstr(getmaxy(stdscr) - 2, 0, "Нажмите стрелку влево, чтобы листать влево и стрелку впрво, чтобы листать вправо или нажмите q, чтобы выйти");
        mvprintw(getmaxy(stdscr) - 1, 0, "Вы на странице: %i", current_page % page_count);
        refresh();
        int choise = 0;
        do
        {
            if (choise == KEY_LEFT)
            {
                current_page = (current_page - 1);
            }
            else if (choise == KEY_RIGHT)
            {
                current_page = (current_page + 1);
            }
            else if (choise == 'q')
            {
                break;
            }
            clear();
            mvaddstr(0, 0, pages[current_page % page_count].c_str());
            mvaddstr(getmaxy(stdscr) - 2, 0, "Нажмите стрелку влево, чтобы листать влево и стрелку впрво, чтобы листать вправо или нажмите q, чтобы выйти");
            mvprintw(getmaxy(stdscr) - 1, 0, "Вы на странице: %i", current_page % page_count);
            refresh();
            choise = getch();
        } while (choise != 'q');
        clear();
        initscr();
    }

    void theory::start_test()
    {
        clear();
        mvaddstr(0, 0, "Введите имя пользователя");
        echo();
        char username[20];
        mvgetnstr(1, 0, username, 20);
        noecho();
        bool solved[QUESTION_MAX_COUNT];
        uint8_t choose = 0;
        uint8_t question = 0;
        uint8_t solved_count = 0;
        uint8_t total_passed = 1;
        uint8_t user_answer = 0;
        do
        {
            question = rand() % question_count;
        } while (solved[question]);
        std::chrono::system_clock::time_point begin = std::chrono::system_clock::now();
        mvaddstr(0, 0, "Нажимайте стрелку вниз или стрелку вверх, чтобы листать варианты ответа.");
        mvaddstr(1, 0, "Нажимите стрелку вправо, чтобы выбрать вариант ответа");
        mvaddstr(2, 0, "Нажмите enter, чтобы отправить ответ или стрелку влево, чтобы пропустить вопрос и вернуться к нему позже.");
        mvaddstr(3, 0, "Или нажмите q чтобы досрочно выйти из теста.");
        refresh();
        int choise = 0;
        do
        {
            clear();
            mvaddstr(0, 0, "Нажимайте стрелку вниз или стрелку вверх, чтобы листать варианты ответа.");
            mvaddstr(1, 0, "Нажимите стрелку вправо, чтобы выбрать вариант ответа");
            mvaddstr(2, 0, "Нажмите enter, чтобы отправить ответ или стрелку влево, чтобы пропустить вопрос и вернуться к нему позже.");
            mvaddstr(3, 0, "Или нажмите q чтобы досрочно выйти из теста.");
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
                if (!GETBIT(user_answer, choose))
                {
                    SETBIT(user_answer, choose);
                }
                else
                {
                    UNSETBIT(user_answer, choose);
                }
                break;
            case KEY_UP:
                choose = (choose - 1) % options_count[question];
                break;
            case KEY_DOWN:
                choose = (choose + 1) % options_count[question];
                break;
            case '\n':
                if ((answers[question] - user_answer) == 0)
                {
                    solved_count += cost[question];
                    mvaddstr(10, 0, "Ответ верный");
                }
                else
                    mvaddstr(10, 0, "Ответ неверный");
                refresh();
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
            print_question(question, choose, user_answer);
            choise = getch();
        } while (total_passed < 5);
        clear();
        std::chrono::system_clock::time_point end = std::chrono::system_clock::now();
        std::ofstream result;
        result.open("result.dat", std::ios::ate);
        std::time_t t = std::chrono::system_clock::to_time_t(begin);
        result << "[" << std::ctime(&t) << "] " << username;
        if (solved_count < 5 || std::chrono::duration_cast<std::chrono::minutes>(end - begin) > std::chrono::minutes(30))
        {
            mvaddstr(0, 0, "Вы не прошли тест");
            result << " не прошел тест" << std::endl;
        }
        else
        {
            mvaddstr(0, 0, "Вы прошли тест");
            result << " прошел тест" << std::endl;
        }
        refresh();
        getch();
        initscr();
    }

    theory::~theory()
    {
    }

} // namespace DL
