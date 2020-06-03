#include "visualization.h"
#include <pdcurses/curses.h>
#include <memory.h>
#include "utils.h"
#define NODE_WIDTH 30
#define NODE_HEIGHT 6
#define DISTANCE_BETWEEN_NODES 4
#define NODE_SUMMARY_SIZE (DISTANCE_BETWEEN_NODES + NODE_HEIGHT)
#define NODE_ORIGIN (getmaxx(vis_window) / 2)

namespace DL
{

    visualization::visualization(/* args */)
    {
    }

    visualization::~visualization()
    {
    }

    inline void mvwprev_node_link_start(WINDOW *vis_window, int node_id)
    {
        wmove(vis_window, NODE_SUMMARY_SIZE * node_id - DISTANCE_BETWEEN_NODES, NODE_ORIGIN + 10);
    }

    inline void mvwnext_node_link_start(WINDOW *vis_window, int node_id)
    {
        wmove(vis_window, NODE_SUMMARY_SIZE * node_id + NODE_HEIGHT, NODE_ORIGIN - 10);
    }

    void visualization::add_loopback()
    {
        std::array<std::string, 3> windows;
        windows[0] = save_window(desc_window);
        windows[1] = save_window(deb_window);
        windows[2] = save_window(vis_window);
        loopback.push_back(windows);
    }

    void visualization::print_loopback(int loopback_id)
    {
        mvwaddnstr(desc_window, 0, 0, loopback[loopback_id][0].c_str(), loopback[loopback_id][0].size());
        mvwaddnstr(deb_window, 0, 0, loopback[loopback_id][1].c_str(), loopback[loopback_id][1].size());
        mvwaddnstr(vis_window, 0, 0, loopback[loopback_id][2].c_str(), loopback[loopback_id][2].size());
        wrefresh(desc_window);
        wrefresh(deb_window);
        wrefresh(vis_window);
    }

    void visualization::print_node(node *n, int position, int offset = 0)
    {
        int node_y = position * NODE_SUMMARY_SIZE;
        int node_x = getmaxx(vis_window) / 2 - NODE_WIDTH / 2 + offset;
        mvwrect(vis_window, node_y, node_x, NODE_WIDTH, NODE_HEIGHT);
        std::string s_address = std::to_string((uint32_t)n);
        mvwaddnstr(vis_window, node_y + 1, getmaxx(vis_window) / 2 - s_address.size() / 2, s_address.c_str() + offset, s_address.size());
        std::string s_prev = "предыдущий = " + std::to_string((uint32_t)n->previous);
        mvwaddnstr(vis_window, node_y + 2, node_x + 1, s_prev.c_str(), s_prev.size());
        std::string s_next = "следующий = " + std::to_string((uint32_t)n->next);
        mvwaddnstr(vis_window, node_y + 3, node_x + 1, s_next.c_str(), s_next.size());
        std::string s_data = "данные = " + std::to_string(n->data);
        mvwaddnstr(vis_window, node_y + 4, node_x + 1, s_data.c_str(), s_data.size());
        wrefresh(vis_window);
    }

    void visualization::print_list()
    {
        node *n = list;
        int i = 0;
        mvwaddstr(vis_window, i * NODE_SUMMARY_SIZE + 2, 0, "Голова списка ------------>");
        while (n)
        {
            print_node(n, i);
            mvwnext_node_link_start(vis_window, i);
            wvline(vis_window, 'Y', DISTANCE_BETWEEN_NODES);

            mvwprev_node_link_start(vis_window, i);
            if (n->previous)
                wvline(vis_window, '^', DISTANCE_BETWEEN_NODES);
            i++;
            n = n->next;
        }
        mvwaddstr(vis_window, NODE_SUMMARY_SIZE * i, NODE_ORIGIN - 13, "NULLPTR");
        mvwaddstr(vis_window, (i - 1) * NODE_SUMMARY_SIZE + 1, 0, "Хвост списка ------------>");
        wrefresh(vis_window);
    }

    void visualization::print_code(op operation)
    {
        wclear(deb_window);
        mvwaddnstr(deb_window, 0, 1, codes[operation].c_str(), codes[operation].size());
        wrefresh(deb_window);
    }

    void visualization::step(std::string description, int line)
    {
        wclear(desc_window);
        mvwaddnstr(desc_window, 0, 0, description.c_str(), description.size());
        for (int i = 0; i < getmaxy(deb_window); i++)
        {
            mvwaddch(deb_window, i, 0, ' ');
        }
        mvwaddch(deb_window, line, 0, '>');
        mvwaddstr(deb_window, getmaxy(deb_window) - 2, 0, "Нажмите стрелку вправо,чтобы продвинуть визуализацию на шаг вперед, или нажимайте стрелку влево, чтобы смотреть прошлы шаги.");
        wrefresh(desc_window);
        wrefresh(deb_window);
        add_loopback();
        int current_step = loopback.size() - 1;

        int choise;
        while (current_step != loopback.size())
        {
            choise = getch();
            if (choise == KEY_RIGHT)
            {
                current_step++;
                if (current_step != loopback.size())
                {
                    print_loopback(current_step);
                }
            }
            if (choise == KEY_LEFT)
            {
                if (current_step > 0)
                {
                    current_step--;
                    print_loopback(current_step);
                }
            }
        }
    }

    void visualization::restore_defaults()
    {
        wclear(vis_window);
        wclear(deb_window);
        wclear(desc_window);
        print_list();
        wrefresh(vis_window);
        wrefresh(deb_window);
        wrefresh(desc_window);
    }


    node *visualization::v_search(int where)
    {
        step("Указатель указывает на первый элемент.", 3);
        node *n = list;
        int i = 0;
        int arrow_x = getmaxx(vis_window) / 2 + 20;
        while (n && (int)n != where)
        {
            n = n->next;
            mvwaddstr(vis_window, i * NODE_SUMMARY_SIZE + 1, arrow_x, "<--- ptr");
            if (i)
                mvwaddstr(vis_window, (i - 1) * NODE_SUMMARY_SIZE + 1, arrow_x, "        ");
            wrefresh(vis_window);
            step("Перейти к следующему узлу.", 5);
            i++;
        }
        mvwaddstr(vis_window, i * NODE_SUMMARY_SIZE + 1, arrow_x, "<--- ptr");
        if (i)
            mvwaddstr(vis_window, (i - 1) * NODE_SUMMARY_SIZE + 1, arrow_x, "        ");
        wrefresh(vis_window);
        return n;
    }

    void visualization::v_push(int32_t data)
    {
        print_list();
        print_code(op::op_push);
        node *begin = list;
        initalize_node(begin->previous, data);

        print_node(begin->previous, 0, NODE_WIDTH / 2 + NODE_WIDTH);
        print_node(begin, 0);
        mvwhline(vis_window, 2, NODE_ORIGIN + NODE_WIDTH / 2 + 1, '>', NODE_WIDTH / 2 - 1);
        wrefresh(vis_window);
        step("Создать новый узел и указателю на предыдущий первого узла присвоить адрес созданного узла", 2);
        begin->previous->next = begin;
        print_node(begin->previous, 0, NODE_WIDTH / 2 + NODE_WIDTH);
        mvwhline(vis_window, 4, NODE_ORIGIN + NODE_WIDTH / 2 + 1, '<', NODE_WIDTH / 2 - 1);
        wrefresh(vis_window);
        step("Указателю на следующий узел созданного узла присвоить адрес первого узла", 3);
        list = begin->previous;
        wclear(vis_window);
        print_list();
        step("Указателю на начало списка присвоить адрес созданного узла", 4);
        restore_defaults();
        list_size++;
    }

    void visualization::v_pop()
    {
        print_list();           //вывести список
        print_code(op::op_pop); //вывести код в поле с визуализацие кода
        node *begin = list;
        step("Сохранить данные первого узла", 2);
        if (begin->next)
        {
            begin->next->previous = nullptr;
            mvwprev_node_link_start(vis_window, 1);
            wvline(vis_window, ' ', DISTANCE_BETWEEN_NODES);
            mvwclrrect(vis_window, NODE_SUMMARY_SIZE, NODE_ORIGIN - NODE_WIDTH / 2, NODE_WIDTH, NODE_HEIGHT);
            print_node(begin->next, 1);
            step("Если это не последний в списке узел, то указатель на предыдущий следующего за первым узла обнулить", 3);
        }
        node *next = begin->next;
        step("Сохранить указатель на следующий узел первого узла", 4);
        delete begin;
        mvwclrrect(vis_window, 0, NODE_ORIGIN - NODE_WIDTH / 2, NODE_WIDTH + 1, NODE_SUMMARY_SIZE);
        step("Удалить первый узел", 5);
        list = next;
        wclear(vis_window);
        print_list();
        step("Присвоить указателю на начало списка адрес сохраненного узла", 6);
        step("Вернуть сохраненные данные", 6);
        list_size--;
    }

    void visualization::v_insert(int where, int32_t data)
    {
        if (!where)
        {
            v_push(data);
            return;
        }
        print_list();              //вывести список
        print_code(op::op_insert); //вывести код в поле с визуализацие кода
        node *n = v_search(where); //визуализировать поиск
        step("Если узел не существует вернуть ложь.", 8);
        if (!n)
            return;
        node *temp = new node;
        initalize_node(temp, data);
        wclear(vis_window);
        print_list();
        print_node(temp, where, NODE_WIDTH + NODE_WIDTH / 4);
        step("Создать новый узел.", 9);

        mvwvline(vis_window, where * NODE_SUMMARY_SIZE + NODE_HEIGHT, NODE_ORIGIN + NODE_WIDTH + NODE_WIDTH / 4 + 10, 'Y', DISTANCE_BETWEEN_NODES + 4);
        mvwaddch(vis_window, where * NODE_SUMMARY_SIZE + NODE_HEIGHT + DISTANCE_BETWEEN_NODES + 4, NODE_ORIGIN + NODE_WIDTH + NODE_WIDTH / 4 + 10, '+');
        mvwhline(vis_window, where * NODE_SUMMARY_SIZE + NODE_HEIGHT + DISTANCE_BETWEEN_NODES + 4, NODE_ORIGIN + NODE_WIDTH / 2 + 1, '<', NODE_WIDTH + 1);
        wrefresh(vis_window);
        temp->next = n->next;
        print_node(temp, where, NODE_WIDTH + NODE_WIDTH / 4);

        step("Указателю на следующий узел созданного узла присвоить указатель на следующий узел узла с выбранным номером.", 10);

        mvwhline(vis_window, where * NODE_SUMMARY_SIZE + 1, NODE_ORIGIN + NODE_WIDTH / 2 + 1, '<', NODE_WIDTH / 4 - 1);
        print_node(temp, where, NODE_WIDTH + NODE_WIDTH / 4);
        wrefresh(vis_window);
        temp->previous = n;

        step("Указателю на предыдущий узел созданного узла присвоить адрес узла с выбранным номером.", 11);

        if (n->next)
        {
            mvwvline(vis_window, where * NODE_SUMMARY_SIZE + NODE_HEIGHT, NODE_ORIGIN + NODE_WIDTH + NODE_WIDTH / 4 + 7, '^', DISTANCE_BETWEEN_NODES + 2);
            mvwaddch(vis_window, where * NODE_SUMMARY_SIZE + NODE_HEIGHT + DISTANCE_BETWEEN_NODES + 2, NODE_ORIGIN + NODE_WIDTH + NODE_WIDTH / 4 + 7, '+');
            mvwhline(vis_window, where * NODE_SUMMARY_SIZE + NODE_HEIGHT + DISTANCE_BETWEEN_NODES + 2, NODE_ORIGIN + NODE_WIDTH / 2 + 1, '>', NODE_WIDTH - 2);
            n->next->previous = temp;
            print_node(n->next, where + 1);
            mvwprev_node_link_start(vis_window, where + 1);
            wvline(vis_window, ' ', DISTANCE_BETWEEN_NODES);
            wrefresh(vis_window);
            step("Если узел не последний, то указателю на предыдущий узел следующего за выбранным узла присвоить адрес созданного узла.", 13);
        }

        mvwhline(vis_window, where * NODE_SUMMARY_SIZE + 3, NODE_ORIGIN + NODE_WIDTH / 2 + 1, '>', NODE_WIDTH / 4 - 1);
        mvwnext_node_link_start(vis_window, where);
        wvline(vis_window, ' ', DISTANCE_BETWEEN_NODES);
        print_node(n, where);
        wrefresh(vis_window);
        n->next = temp;
        step("Указателю на следующий выбранного узла присвоить адрес созданного узла.", 14);
        step("Вернуть истину.", 15);
        restore_defaults();
        list_size++;
    }

    void visualization::v_remove(int where)
    {
        if (!where)
        {
            v_pop();
            return;
        }

        print_list();
        print_code(op::op_remove);
        node *n = v_search(where);
        step("Если узел не существует вернуть ложь.", 8);
        if (!n)
            return;

        n->previous->next = n->next;
        mvwhline(vis_window, (where - 1) * NODE_SUMMARY_SIZE + 1, NODE_ORIGIN - NODE_WIDTH, '<', NODE_WIDTH / 2);
        mvwvline(vis_window, (where - 1) * NODE_SUMMARY_SIZE + 1, NODE_ORIGIN - NODE_WIDTH - 1, 'Y', NODE_SUMMARY_SIZE * 2);
        mvwhline(vis_window, (where + 1) * NODE_SUMMARY_SIZE + 1, NODE_ORIGIN - NODE_WIDTH, '>', NODE_WIDTH / 2);
        mvwaddch(vis_window, (where - 1) * NODE_SUMMARY_SIZE + 1, NODE_ORIGIN - NODE_WIDTH - 1, '+');
        mvwaddch(vis_window, (where + 1) * NODE_SUMMARY_SIZE + 1, NODE_ORIGIN - NODE_WIDTH - 1, '+');
        mvwnext_node_link_start(vis_window, where - 1);
        wvline(vis_window, ' ', DISTANCE_BETWEEN_NODES);
        wrefresh(vis_window);
        print_node(n->previous, where - 1);
        step("Указателю на следующий предыдущего за данным узла присвоить указатель на следующий данного узла.", 10);

        if (n->next)
        {
            n->next->previous = n->previous;
            mvwhline(vis_window, (where - 1) * NODE_SUMMARY_SIZE + 1, NODE_ORIGIN + NODE_WIDTH / 2 + 1, '<', NODE_WIDTH / 2);
            mvwvline(vis_window, (where - 1) * NODE_SUMMARY_SIZE + 1, NODE_ORIGIN + NODE_WIDTH + 1, '^', NODE_SUMMARY_SIZE * 2);
            mvwhline(vis_window, (where + 1) * NODE_SUMMARY_SIZE + 1, NODE_ORIGIN + NODE_WIDTH / 2 + 1, '>', NODE_WIDTH / 2);
            mvwaddch(vis_window, (where - 1) * NODE_SUMMARY_SIZE + 1, NODE_ORIGIN + NODE_WIDTH + 1, '+');
            mvwaddch(vis_window, (where + 1) * NODE_SUMMARY_SIZE + 1, NODE_ORIGIN + NODE_WIDTH + 1, '+');
            mvwprev_node_link_start(vis_window, where + 1);
            wvline(vis_window, ' ', DISTANCE_BETWEEN_NODES);
            wrefresh(vis_window);
            print_node(n->next, where + 1);
        }
        step("Если выбранный узел не последний то указателю на предыдущий следующего за данным узла присвоить указатель на предыдущий данного узла.", 12);

        mvwclrrect(vis_window, (where - 1) * NODE_SUMMARY_SIZE + NODE_HEIGHT, NODE_ORIGIN - NODE_WIDTH / 2, NODE_WIDTH + 1, NODE_SUMMARY_SIZE + DISTANCE_BETWEEN_NODES);
        wrefresh(vis_window);
        step("Удалить данный узел.", 12);
        wclear(vis_window);
        print_list();
        step("Вернуть истину.", 14);
        list_size--;
    }

    void visualization::visualization_start()
    {
        vis_window = create_newwin(LINES, COLS * 5 / 8, 0, COLS * 3 / 8);
        desc_window = create_newwin(LINES / 2, COLS * 3 / 8, 0, 0);
        deb_window = create_newwin(LINES / 2, COLS * 3 / 8, LINES / 2, 0);

        mvwaddstr(desc_window, getmaxy(desc_window) - 2, 0, "Во время визуализации нажмите стрелку вправо, чтобы продвинуться на шаг вперед или стрелку влево, чтобы посмотреть предыдущие шаги (только посмотреть)");
        wrefresh(desc_window);
        initalize_node(list, 1);
        list_size = 1;
        char choise;
        print_list();
        do
        {
            wclear(desc_window);
            mvwaddstr(desc_window, 0, 0, "Введите номер операции");
            mvwaddstr(desc_window, 1, 0, "1.Вставить элемент в начало списка");
            mvwaddstr(desc_window, 2, 0, "2.Удалить элмент из начала списка");
            mvwaddstr(desc_window, 3, 0, "3.Вставить элемент в середине списка");
            mvwaddstr(desc_window, 4, 0, "4.Удалить элмент из середины списка");
            mvwaddstr(desc_window, 5, 0, "0.Выйти из визуализации");
            wrefresh(desc_window);
            choise = getch();
            char buffer[10];
            memset(buffer, 0, 10);

            echo();
            switch (choise)
            {
            case '1':
            {
                mvwaddstr(desc_window, 6, 0, "Введите, что вставлять ");
                wrefresh(desc_window);
                mvwgetnstr(desc_window, 7, 0, buffer, 10);
                int data = atoi(buffer);
                wclear(desc_window);
                wrefresh(desc_window);
                noecho();
                v_push(data);
            }
            break;
            case '2':
                noecho();
                v_pop();
                break;
            case '3':
            {
                mvwaddstr(desc_window, 6, 0, "Введите, что вставлять ");
                wrefresh(desc_window);
                mvwgetnstr(desc_window, 7, 0, buffer, 10);
                int data = atoi(buffer);
                mvwaddstr(desc_window, 7, 0, "       ");
                mvwaddstr(desc_window, 6, 0, "Введите, где вставлять ");
                wrefresh(desc_window);
                int where;
                mvwgetnstr(desc_window, 7, 0, buffer, 10);
                where = atoi(buffer);
                noecho();
                v_insert(where - '0', data);
            }
            break;
            case '4':
            {
                mvwaddstr(desc_window, 6, 0, "Введите, где удалять ");
                wrefresh(desc_window);
                int where;
                mvwgetnstr(desc_window, 7, 0, buffer, 10);
                where = atoi(buffer);
                noecho();
                v_remove(where - '0');
                break;
            }
            case '0':
            {
                break;
            }
            }
            noecho();
        } while (list_size != 0 && choise != '0');
        initscr();
    }

} // namespace DL
