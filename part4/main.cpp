#include <iostream>
#include <string>

#include "driver.hpp"
#include "ast.hpp"

using namespace std;
using namespace AST;

enum class Mode
{
    LEX,
    PARSE,
    SEMANTIC_ANALYSIS,
    CODE_GENERATION,
    EXTENDED
};

static const map<string, Mode> flag_to_mode = {
    {"-l", Mode::LEX},
    {"-p", Mode::PARSE},
    {"-c", Mode::SEMANTIC_ANALYSIS},
    {"-i", Mode::CODE_GENERATION},
    {"-e", Mode::EXTENDED}
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
        cerr << "Usage: " << argv[0] << " [-l|-p|-c] <source_file>" << endl;
        return -1;
    }

    VSOP::Driver driver = VSOP::Driver(source_file);

    int res;
    switch (mode)
    {
    case Mode::LEX:
        res = driver.lex();

        driver.print_tokens();

        return res;

    case Mode::PARSE:
        res = driver.parse();

        if (res == 0){
            driver.print_ast();
            driver.delete_ast();
        } 

        return res;

    case Mode::SEMANTIC_ANALYSIS:
        res = driver.parse();

        if (res == 0){
            res = driver.semantic_analysis();
            driver.print_ast();
            driver.delete_ast();
            driver.delete_type_table();
        }

        return res;
    case Mode::EXTENDED:
    case Mode::CODE_GENERATION:
        res = driver.parse();

        if (res != 0)
            return res;

        res = driver.semantic_analysis();
        driver.delete_type_table();

        if (res != 0)
            return res;

        // put code generation code here

        driver.delete_ast();

        return res;
    }

    return 0;
}