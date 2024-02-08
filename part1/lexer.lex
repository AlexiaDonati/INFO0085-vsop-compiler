%{
    /* Includes */
    #include "parser.tab.h"
    #include <iostream>
    #include <string>

    #include <list>
    #include <algorithm>
%}

/* Flex options
 * - noyywrap: yylex will not call yywrap() function
 * - nounput: do not generate yyunput() function
 * - noinput: do not generate yyinput() function
 * - batch: tell Flex that the lexer will not often be used interactively
 */
%option noyywrap nounput noinput batch

%{
    // Restart: move the first cursor to the last position
    # define LOCATION_STEP(Loc)                   \
        (Loc).first_column = (Loc).last_column;   \
        (Loc).first_line = (Loc).last_line;

    // The following code is executed each time a token is read
    #define YY_USER_ACTION \
        LOCATION_STEP(yylloc); \
        for (int i = 0 ; i < yyleng; i++) { \
            /* If a newline is read, the line number is incremented and the column number is reset to 1 */ \
            if(yytext[i] == '\n') { \
                yylloc.last_line++; \
                yylloc.last_column = 1; \
            } \
            /* Otherwise, the column number is incremented */ \
            else{ \
                yylloc.last_column++; \
            } \
        }

    /* In practice, a hash table contains all keywords */
    std::list<std::string> keywords = {
        "and", 
        "bool", 
        "class", 
        "do", 
        "else", 
        "extends", 
        "false", 
        "if", 
        "in", 
        "int32", 
        "isnull", 
        "let", 
        "new", 
        "not", 
        "self", 
        "string", 
        "then", 
        "true", 
        "unit", 
        "while"
    }; // To replace with a hash table in following steps
    
    /* Counts the level of nested comments at current position (starting at 0) */
    int started_comment = 0;

    std::string string_buffer = "";
    std::string hex = "";

    int string_start_column = 0;
    int string_start_line = 0;
%}

/* Lexical Structure */
lowercase_letter    [a-z]
uppercase_letter    [A-Z]
letter              {lowercase_letter}|{uppercase_letter}
bin_digit           [0-1]
digit               {bin_digit}|[2-9]
hex_digit           {digit}|[a-f]|[A-F]

/* whitespace = { " " | tab | lf | ff | cr } */
blank               [ \t\n\f\r]
white_space          {blank}+

/* Single-line comments are introduced by // and continue up to the next line feed \n */
single_line_comment "//"[^\n]*

%x multi_line_comment

base_10_number      {digit}+
base_16_number      "0x"{hex_digit}+

integer_literal     {base_10_number}|{base_16_number}

/* type identifiers always begin with an uppercase letter, and can contain letters, digits and underscores */
type_identifier     {uppercase_letter}({letter}|{digit}|"_")*

/* object identifiers begin with a lowercase letter, and can contain letters, digits and underscores */
object_identifier   {lowercase_letter}({letter}|{digit}|"_")*

escape_sequence     b|t|n|r|\"|\\|x{hex_digit}{hex_digit}|\n[ \t]*
escaped_char        \\{escape_sequence}

/* A string cannot contain a literal line feed, or the end-of-file */
regular_char        [^\"\n\0\\]
string_literal      "{regular_char}|{escaped_char}"

%x string

operator            "{"|"}"|"("|")"|":"|";"|","|"+"|"-"|"*"|"/"|"^"|"."|"="|"<"|"<="|"<-"

%% /* Rules */

{white_space}           /* eat up whitespace */
{single_line_comment}   /* eat up single-line comments */

<INITIAL>"(*" {
    started_comment = 1;
    BEGIN(multi_line_comment);  /* As soon as (* is read, another lexical analyzer that only reads (* and *) is called */  
}

<INITIAL>"*)" { /* A comment must be open before being closed */
    //TODO : error
    std::cout << "error: A comment must be open before being closed" << std::endl;
}

<multi_line_comment>"(*" {
    started_comment++;
}

<multi_line_comment>"*)" {
    started_comment--;
    if (started_comment == 0)
        BEGIN(INITIAL);     
}

<multi_line_comment>[^<<EOF>>]    /* eat up multi-line comments */

<multi_line_comment><<EOF>> { /* All comment must be closed before the end of file */
    //TODO : error
    std::cout << "error: All comment must be closed before the end of file" << std::endl;
    BEGIN(INITIAL);
}

{integer_literal} {  /* For integer literals, output the decimal value (whatever the input format was) */
    std::cout << yylloc.first_line << "," << yylloc.first_column << "," << "integer-literal" << ",";
    if(yytext[0] == '0' && yytext[1] == 'x'){
        std::cout << std::hex << std::stoi(yytext, nullptr, 16) << std::endl;
    }
    else{
        std::cout << std::stoi(yytext) << std::endl;
    }
}

{type_identifier} { 
    std::cout << yylloc.first_line << "," << yylloc.first_column << "," << "type-identifier" << "," << yytext << std::endl; 
}

{object_identifier} {
    auto it = find(keywords.begin(), keywords.end(), yytext); /* Once an identifier/keyword is read, a table lookup decides if it is an identifier or a keyword */
    if(it != keywords.end()){
        std::cout << yylloc.first_line << "," <<  yylloc.first_column << "," << yytext << std::endl;
    }
    else{
        std::cout << yylloc.first_line << "," <<  yylloc.first_column << "," << "object-identifier" << "," << yytext << std::endl; 
    }
}

\" {
    string_buffer = "";
    string_start_column = yylloc.first_column;
    string_start_line = yylloc.first_line;
    BEGIN(string);
}

<string>{escaped_char} {
    switch(yytext[1]){
        case 'b': string_buffer += "\\x08"; break;
        case 't': string_buffer += "\\x09"; break;
        case 'n': string_buffer += "\\x0a"; break;
        case 'r': string_buffer += "\\x0d"; break;
        case '"': string_buffer += "\\x22"; break;
        case '\\': string_buffer += "\\x5c"; break;
        case 'x': 
            hex.assign(yytext, yyleng-2, 2);
            string_buffer += std::stoi(hex, 0, 16);
            break;
        case '\n': break;
        default: string_buffer += yytext[1]; break;
    }
} 

<string>{regular_char}+ {string_buffer += yytext;}

<string>\" {
    std::cout << string_start_line << "," << string_start_column << "," << "string-literal" << ",";
    std::cout << "\"" << string_buffer << "\"" << std::endl;
    BEGIN(INITIAL);
}

{operator} {
    std::cout << yylloc.first_line << "," << yylloc.first_column << ",";
    switch(yytext[0]){
        case '{': std::cout << "lbrace" << std::endl; break;
        case '}': std::cout << "rbrace" << std::endl; break;
        case '(': std::cout << "lpar" << std::endl; break;
        case ')': std::cout << "rpar" << std::endl; break;
        case ':': std::cout << "colon" << std::endl; break;
        case ';': std::cout << "semicolon" << std::endl; break;
        case ',': std::cout << "comma" << std::endl; break;
        case '+': std::cout << "plus" << std::endl; break;
        case '-': std::cout << "minus" << std::endl; break;
        case '*': std::cout << "times" << std::endl; break;
        case '/': std::cout << "div" << std::endl; break;
        case '^': std::cout << "pow" << std::endl; break;
        case '.': std::cout << "dot" << std::endl; break;
        case '=': std::cout << "equal" << std::endl; break;
        case '<': {
            if(yytext[1] == '='){
                std::cout << "lower-equal" << std::endl;
            }
            else if(yytext[1] == '-'){
                std::cout << "assign" << std::endl;
            }
            else{
                std::cout << "lower" << std::endl;
            }
            break;
        }
    }
}

%%



