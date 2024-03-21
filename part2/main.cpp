#include <iostream>
#include <string>

#include "driver.hpp"
#include "ast.hpp"

using namespace std;
using namespace AST;

/*
int main(){
    string* str;

    Integer* integer = new Integer(42, 0, "int", 78);

    Let* let = new Let(0,0,"test","name","type",integer,integer);

    Print_visitor* print_visitor = new Print_visitor();

    List<Expr>* list = new List<Expr>();

    list->add((Expr*) integer);
    list->add((Expr*) let);

    Block* block = new Block(0, 0, "block", list);

    str = (string*)block->accept(print_visitor);
    
    //cout << *str << endl;

    List<Class>* list_class = new List<Class>();

    Program* program = new Program(0, 0, "program", list_class);

    str = (string*)program->accept(print_visitor);

    cout << *str << endl;

    delete str;
}
*/

enum class Mode
{
    LEX,
    PARSE
};

static const map<string, Mode> flag_to_mode = {
    {"-l", Mode::LEX},
    {"-p", Mode::PARSE},
};

int main(int argc, char const *argv[])
{
    Mode mode;
    string source_file;

    if (argc == 2)
    {
        mode = Mode::PARSE;
        source_file = argv[1];
    }
    else if (argc == 3)
    {
        if (flag_to_mode.count(argv[1]) == 0)
        {
            cerr << "Invalid mode: " << argv[1] << endl;
            return -1;
        }
        mode = flag_to_mode.at(argv[1]);
        source_file = argv[2];
    }
    else
    {
        cerr << "Usage: " << argv[0] << " [-l|-p] <source_file>" << endl;
        return -1;
    }

    VSOP::Driver driver = VSOP::Driver(source_file);

    int res;
    switch (mode)
    {
    case Mode::LEX:
        res = driver.lex();

        if (res == 0)
            driver.print_tokens();

        return res;

    case Mode::PARSE:
        res = driver.parse();

        string *str = nullptr;
        
        if (res == 0)
            str = driver.get_ast();

        cout << *str << endl;

        return res;
    }

    return 0;
}