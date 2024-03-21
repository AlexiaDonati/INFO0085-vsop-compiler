%skeleton "lalr1.cc" // -*- C++ -*-
%language "c++"
%require "3.7.5"
%locations

%defines

// Put parser inside a namespace
%define api.namespace {VSOP}

// Give the name of the parser class
%define api.parser.class {Parser}

// Force the token kind enum (used by the lexer) and the symbol kind enum
// (used by the parser) to use the same value for the tokens.
// (e.g. '+' will be represented by the same integer value in both enums.)
%define api.token.raw

// Tokens contain their type, value and location
// Also allow to use the make_TOKEN functions
%define api.token.constructor

// Allow to use C++ objects as semantic values
%define api.value.type variant

// Add some assertions.
%define parse.assert

// C++ code put inside header file
%code requires { 
    #include <string>

    namespace VSOP
    {
        class Driver;
    }

    #include "ast.hpp"

    struct ClassBody {
        AST::List<AST::Field> *fields;
        AST::List<AST::Method> *methods;
    };
}


// Add an argument to the parser constructor
%parse-param {VSOP::Driver &driver}

%code {
    #include "driver.hpp"

    using namespace std;
}

// Token and symbols definitions
%token  
    AND "and"
    BOOL "bool"
    CLASS "class"
    DO "do"
    ELSE "else" 
    EXTENDS "extends" 
    FALSE "false"
    IF "if"
    IN "in" 
    INT32 "int32" 
    ISNULL "isnull"
    LET "let"
    NEW "new" 
    NOT "not" 
    SELF "self" 
    STRING "string" 
    THEN "then"
    TRUE "true" 
    UNIT "unit" 
    WHILE "while"
;

%token
    LBRACE "{"
    RBRACE "}"
    LPAR "("
    RPAR ")"
    COLON ":"
    SEMICOLON ";"
    COMMA ","
    PLUS  "+"
    MINUS  "-"
    TIMES "*"
    DIV "/"
    POW "^"
    DOT "."
    EQUAL "="
    LOWER_EQUAL "<="
    ASSIGN  "<-"
    LOWER "<"
;

// For some symbols, need to store a value
%token <int> INTEGER_LITERAL "integer-literal"
%token <std::string> TYPE_IDENTIFIER "type-identifier"
%token <std::string> KEYWORDS "keywords"
%token <std::string> OBJECT_IDENTIFIER "object-identifier"
%token <std::string> STRING_LITERAL "string_literal"

%nterm 
    <AST::Program *> program
    <AST::List<AST::Class> *> class_list

    <AST::Class *> class 
    <ClassBody *> class-body

    <AST::Field *> field
    <AST::Method *> method

    <std::string> type

    <AST::Formal *> formal
    <AST::List<AST::Formal>*> formals

    <AST::Expr *> expr literal
    <AST::List<AST::Expr>*> expr_list args

    <AST::Block *> block
;

// Precedence
%precedence IF THEN WHILE LET DO IN
%precedence ELSE

%right ASSIGN
%left AND
%right NOT
%nonassoc LOWER LOWER_EQUAL EQUAL
%left PLUS MINUS TIMES DIV
%right ISNULL POW
%left DOT

%% 
// Grammar rules

%start program;

program:    class program 
            { 
                std::cout << "Program" << std::endl;
                $2->get_class_list()->add($1);
                $$ = $2; 
            }
            | /* empty */ 
            { 
                std::cout << "Empty Program" << std::endl;
                AST::List<AST::Class>* classes = new AST::List<AST::Class>(); 
                $$ = new AST::Program(@$.begin.line, @$.begin.column, driver.get_source_file(), classes);
                driver.set_ast($$);
            };
            

class:  CLASS TYPE_IDENTIFIER LBRACE class-body RBRACE 
        { 
            std::cout << "Class" << std::endl;
            $$ = new AST::Class(@$.begin.line, @$.begin.column, driver.get_source_file(), $2, "Object", $4->fields, $4->methods);
        };
        | CLASS TYPE_IDENTIFIER EXTENDS TYPE_IDENTIFIER LBRACE class-body RBRACE 
        { 
            std::cout << "Class" << std::endl;
            $$ = new AST::Class(@$.begin.line, @$.begin.column, driver.get_source_file(), $2, $4, $6->fields, $6->methods); 
        };

class-body: field class-body 
            { 
                std::cout << "class-body" << std::endl;
                $2->fields->add($1);
                $$ = $2; 
            }
            | method class-body 
            { 
                std::cout << "class-body" << std::endl;
                $2->methods->add($1);
                $$ = $2; 
            }
            | /* empty */ { $$ = new ClassBody(); };


field : OBJECT_IDENTIFIER COLON type SEMICOLON 
        { 
            $$ = new AST::Field(@$.begin.line, @$.begin.column, driver.get_source_file(), $1, $3, nullptr); 
        };
        | OBJECT_IDENTIFIER COLON type ASSIGN expr SEMICOLON 
        { 
            $$ = new AST::Field(@$.begin.line, @$.begin.column, driver.get_source_file(), $1, $3, $5); 
        };

method: OBJECT_IDENTIFIER LPAR formals RPAR COLON type block 
        { 
            $$ = new AST::Method(@$.begin.line, @$.begin.column, driver.get_source_file(), $1, $3, $6, $7); 
        };

type:   TYPE_IDENTIFIER { $$ = $1; }
        | INT32 { $$ = std::string("int32"); }
        | BOOL { $$ = std::string("bool"); }
        | STRING { $$ = std::string("string"); }
        | UNIT { $$ = std::string("unit"); };

formals:    formal COMMA formals 
            { 
                $3->add($1);
                $$ = $3;
            }
            | formal 
            { 
                $$->add($1);
            }
            | /* empty */ { $$ = new AST::List<AST::Formal>(); };

formal: OBJECT_IDENTIFIER COLON type
        { 
            $$ = new AST::Formal(@$.begin.line, @$.begin.column, driver.get_source_file(), $1, $3); 
        };

block:  LBRACE expr_list RBRACE 
        { 
            $$ = new AST::Block(@$.begin.line, @$.begin.column, driver.get_source_file(), $2); 
            delete $2;
        };

expr_list:  expr SEMICOLON expr_list 
            { 
                $3->add($1); 
                $$ = $3; 
            }
            | expr 
            { 
                $$->add($1);
            }
            | /* empty */ { $$ = new AST::List<AST::Expr>(); };

expr:   IF expr THEN expr 
        { 
            $$ = new AST::If(@$.begin.line, @$.begin.column, driver.get_source_file(), $2, $4, nullptr);
        }
        | IF expr THEN expr ELSE expr 
        { 
            $$ = new AST::If(@$.begin.line, @$.begin.column, driver.get_source_file(), $2, $4, $6); 
        }

        | WHILE expr DO expr 
        { 
            $$ = new AST::While(@$.begin.line, @$.begin.column, driver.get_source_file(), $2, $4); 
        }

        | LET OBJECT_IDENTIFIER COLON type IN expr 
        { 
            $$ = new AST::Let(@$.begin.line, @$.begin.column, driver.get_source_file(), $2, $4, nullptr, $6); 
        }
        | LET OBJECT_IDENTIFIER COLON type ASSIGN expr IN expr 
        { 
            $$ = new AST::Let(@$.begin.line, @$.begin.column, driver.get_source_file(), $2, $4, $6, $8); 
        }

        | OBJECT_IDENTIFIER ASSIGN expr 
        { 
            $$ = new AST::Assign(@$.begin.line, @$.begin.column, driver.get_source_file(), $1, $3); 
        }

        | NOT expr 
        { 
            $$ = new AST::Unop(@$.begin.line, @$.begin.column, driver.get_source_file(), "not", $2); 
        }
        | MINUS expr 
        { 
            $$ = new AST::Unop(@$.begin.line, @$.begin.column, driver.get_source_file(), "-", $2); 
        }
        | ISNULL expr 
        { 
            $$ = new AST::Unop(@$.begin.line, @$.begin.column, driver.get_source_file(), "isnull", $2); 
        }

        | expr EQUAL expr 
        { 
            $$ = new AST::Binop(@$.begin.line, @$.begin.column, driver.get_source_file(), "=", $1, $3); 
        }
        | expr LOWER expr 
        { 
            $$ = new AST::Binop(@$.begin.line, @$.begin.column, driver.get_source_file(), "<", $1, $3); 
        }
        | expr LOWER_EQUAL expr 
        { 
            $$ = new AST::Binop(@$.begin.line, @$.begin.column, driver.get_source_file(), "<=", $1, $3); 
        }
        | expr PLUS expr 
        { 
            $$ = new AST::Binop(@$.begin.line, @$.begin.column, driver.get_source_file(), "+", $1, $3); 
        }
        | expr MINUS expr 
        { 
            $$ = new AST::Binop(@$.begin.line, @$.begin.column, driver.get_source_file(), "-", $1, $3); 
        }
        | expr TIMES expr 
        { 
            $$ = new AST::Binop(@$.begin.line, @$.begin.column, driver.get_source_file(), "*", $1, $3); 
        }
        | expr DIV expr 
        { 
            $$ = new AST::Binop(@$.begin.line, @$.begin.column, driver.get_source_file(), "/", $1, $3); 
        }
        | expr POW expr 
        { 
            $$ = new AST::Binop(@$.begin.line, @$.begin.column, driver.get_source_file(), "^", $1, $3); 
        }
        | expr AND expr 
        { 
            $$ = new AST::Binop(@$.begin.line, @$.begin.column, driver.get_source_file(), "and", $1, $3); 
        }
        
        | OBJECT_IDENTIFIER LPAR args RPAR 
        { 
            $$ = new AST::Call(@$.begin.line, @$.begin.column, driver.get_source_file(), "self", $1, $3); 
        }
        | OBJECT_IDENTIFIER DOT OBJECT_IDENTIFIER LPAR args RPAR 
        { 
            $$ = new AST::Call(@$.begin.line, @$.begin.column, driver.get_source_file(), $1, $3, $5); 
        }

        | NEW TYPE_IDENTIFIER 
        { 
            $$ = new AST::New(@$.begin.line, @$.begin.column, driver.get_source_file(), $2); 
        }

        | OBJECT_IDENTIFIER { $$ = new AST::Object(@$.begin.line, @$.begin.column, driver.get_source_file(), $1); }
        
        | SELF { $$ = new AST::Self(@$.begin.line, @$.begin.column, driver.get_source_file()); }

        | literal { $$ = $1; }

        | LPAR RPAR { $$ = new AST::Unit(@$.begin.line, @$.begin.column, driver.get_source_file()); }
        | LPAR expr RPAR { $$ = $2; };

args:   expr COMMA args 
        { 
            $3->add($1); 
            $$ = $3; 
        }
        | expr 
        { 
            $$->add($1);
        }
        | /* empty */ { $$ = new AST::List<AST::Expr>(); };

literal:    INTEGER_LITERAL { $$ = new AST::Integer(@$.begin.line, @$.begin.column, driver.get_source_file(), $1); }
            | TRUE { $$ = new AST::Boolean(@$.begin.line, @$.begin.column, driver.get_source_file(), true); }
            | FALSE { $$ = new AST::Boolean(@$.begin.line, @$.begin.column, driver.get_source_file(), false); }
            | STRING_LITERAL { $$ = new AST::String(@$.begin.line, @$.begin.column, driver.get_source_file(), $1); };

%%

// User code
void VSOP::Parser::error(const location_type& l, const std::string& m)
{
    const position &pos = l.begin;

    cerr << *(pos.filename) << ":"
         << pos.line << ":" 
         << pos.column << ": "
         << m
         << endl;
}


