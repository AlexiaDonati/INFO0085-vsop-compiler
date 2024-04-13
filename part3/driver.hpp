#ifndef _DRIVER_HPP
#define _DRIVER_HPP

#include <string>
#include <vector>
#include <map>

#include "ast.hpp"
#include "parser.hpp"

// Give prototype of yylex() function, then declare it.
#define YY_DECL VSOP::Parser::symbol_type yylex()
YY_DECL;

namespace VSOP
{
    class Driver
    {
    public:
        /**
         * @brief Construct a new Driver.
         *
         * @param _source_file The file containing the source code.
         */
        Driver(const std::string &_source_file) : source_file(_source_file) {}

        /**
         * @brief Get the source file.
         *
         * @return const std::string& The source file.
         */
        const std::string &get_source_file() { return source_file; }

        /**
         * @brief Set the AST.
         * 
         * @param _ast The AST.
        */
        void set_ast(AST::Program *_ast) { ast = _ast; }

        /**
         * @brief Print the AST.
         */
        void print_ast() {
            AST::Print_visitor visitor;
            std::string* str = (std::string*) ast->accept(&visitor);
            std::cout << *str << std::endl;
            delete str;
        }

        /**
         * @brief Delete the AST.
         */
        void delete_ast() {
            delete ast;
        }
        
        /**
         * @brief Run the lexer on the source file.
         *
         * @return int 0 if no lexical error.
         */
        int lex();

        /**
         * @brief Run the parser on the source file and compute the result.
         *
         * @return int 0 if no syntax or lexical error.
         */
        int parse();

        /**
         * @brief Print all the tokens.
         */
        void print_tokens();

    private:
        /**
         * @brief The source file.
         */
        std::string source_file;

        /**
         * @brief The parser.
         */
        VSOP::Parser *parser;
        
        /**
         * @brief Store the tokens.
         */
        std::vector<Parser::symbol_type> tokens;

        /**
         * @brief The AST.
         */
        AST::Program *ast;

        /**
         * @brief Start the lexer.
         */
        void scan_begin();

        /**
         * @brief Stop the lexer.
         */
        void scan_end();
    };
}

#endif
