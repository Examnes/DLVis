#if !defined(THEORY_H)
#define THEORY_H
#define QUESTION_MAX_COUNT 10
#define THEORY_PAGE_MAX_COUNT 10
#include <string>
namespace DL
{
    
    class theory
    {
    private:
        std::string pages[THEORY_PAGE_MAX_COUNT];
        std::string questions[QUESTION_MAX_COUNT];
        uint8_t answers[QUESTION_MAX_COUNT];
        std::string options[QUESTION_MAX_COUNT][8];
        uint16_t options_count[QUESTION_MAX_COUNT];
        bool solved[QUESTION_MAX_COUNT];
        uint16_t page_count;
        uint16_t question_count;
        uint16_t current_page;
        void print_page(uint8_t page);
        void print_question(uint8_t question,uint8_t choose,uint8_t answer);
    public:
        theory(std::string theorybase, std::string questionbase);
        void start_read();
        void start_test();
        ~theory();
    };
} // namespace DL

#endif // THEORY_H




