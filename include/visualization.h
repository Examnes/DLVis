#if !defined(VISUALIZATION_H)
#define VISUALIZATION_H

#include "node.h"
#include "pdcurses/curses.h"
#include <vector>
#include <array>

namespace DL
{
    class visualization
    {
    private:
        std::vector<std::array<std::string,3>> loopback;
        void add_loopback();
        void print_loopback(int loopback_id);

        WINDOW* vis_window;
        WINDOW* deb_window;
        WINDOW* desc_window;

        node* list;
        int list_size;

        void print_node(node* n,int position,int offset);
        void print_list();

        void restore_defaults();

        node* v_search(int where);
        void v_push(int32_t data);
        void v_pop();
        void v_insert(int where,int32_t data);
        void v_remove(int where);
        void v_push_f(int32_t data);
        void v_pop_f();
        void v_search_data(int32_t data);

        enum op
        {
            op_push,
            op_pop,
            op_insert,
            op_remove,
            op_push_f,
            op_pop_f,
            op_search
        };

        void print_code(op operation);
        void step(std::string description,int line);

std::string codes[7] = 
{
    R"( void push_back(uint32_t data)
    {
        node* begin = list.begin;
        if(begin)
        {
            begin->previous = node_init(data);
            begin->previous->next = begin;
            list = begin->previous;
        }else
            list = node_init(data);
    })",
    R"( uint32_t pop_back()
    {
        node* begin = list.begin;
        uint32_t data = begin->data;
        if(begin->next) begin->next->previous = nullptr;
        node* next = begin->next;
        delete begin;
        list = next;
        return data;
    })",
    R"( bool insert(size_t where, uint32_t data)
    {
        node* begin = list.begin;
        while (begin != where && begin)
        {
            begin = begin->next;
        }
        if (begin!=where)
            return false;
        node *temp = node_init(data);
        temp->next = begin->next;
        temp->previous = begin;
        if (begin->next)
            begin->next->previous = temp;
        begin->next = temp;
        return true;
    })",
    R"( bool remove(size_t where)
    {
        node* begin = list.begin;
        while (begin != where && begin)
        {
            begin = begin->next;
        }
        if (begin!=where)
            return false;
        if (begin->previous)
            begin->previous->next = begin->next;
        if (begin->next)
            begin->next->previous = begin->previous;
        delete begin;
        return true;
    })",
    R"( void push_front(uint32_t data)
    {
        node* end = list.end;
        if(end)
        {
            end->next = node_init(data);
            end->next->previous = end;
            list.end = end->next;
        }else
        {
            list.end = node_init(data);
            list.begin = list.end;
        }
    })",
    R"( uint32_t pop_front()
    {
        node* end = list.end;
        list.end = end->previous;
        if(list.end) list.end->next = nullptr;
        delete end;
    })",
    R"( node* search(uint32_t data)
    {
        node* n = list.begin;
        while(n || n->data != data)
            n = n->next;
        return n;
    })",
};

    public:
        visualization(/* args */);
        ~visualization();
        void visualization_start();
    };
} // namespace DL


#endif // VISUALIZATION_H
