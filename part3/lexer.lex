%{
    /* Includes */
    #include <string>
    #include <stack>
    #include <list>
    #include <algorithm>

    #include "parser.hpp"
    #include "driver.hpp"
%}

    /* Flex options
     * - noyywrap: yylex will not call yywrap() function
     * - nounput: do not generate yyunput() function
     * - noinput: do not generate yyinput() function
     * - batch: tell Flex that the lexer will not often be used interactively
     */
%option noyywrap nounput noinput batch

%{
    /* Code to include at the beginning of the lexer file. */
    using namespace std;
    using namespace VSOP;

    // Print an lexical error message.
    static void print_error(const position &pos,
                            const string &m);

    // Code run each time a pattern is matched.
    #define YY_USER_ACTION  loc.columns(yyleng);

    // Global variable used to maintain the current location.
    location loc;

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

    location string_start;

    stack<location> comm_start_stack;
%}

    /* Definitions */
/* Lexical Structure */
lowercase_letter    [a-z]
uppercase_letter    [A-Z]
letter              {lowercase_letter}|{uppercase_letter}
bin_digit           [0-1]
digit               {bin_digit}|[2-9]
hex_digit           {digit}|[a-f]|[A-F]

/* whitespace = { " " | tab | lf | ff | cr } */
blank               [ \t\f\r]
white_space          {blank}+

/* Single-line comments are introduced by // and continue up to the next line feed \n */
single_line_comment "//"[^\n]*

%x multi_line_comment
%x STRING
%x NEW_LINE_STRING

base_10_number      {digit}+
base_16_number      "0x"{hex_digit}+

integer_literal     {base_10_number}|{base_16_number}

/* type identifiers always begin with an uppercase letter, and can contain letters, digits and underscores */
type_identifier     {uppercase_letter}({letter}|{digit}|"_")*

/* object identifiers begin with a lowercase letter, and can contain letters, digits and underscores */
object_identifier   {lowercase_letter}({letter}|{digit}|"_")*

identifier {type_identifier}|{object_identifier}

escape_sequence     b|t|n|r|\"|\\|x{hex_digit}{hex_digit}|\n
escaped_char        \\{escape_sequence}

/* A string cannot contain a literal line feed, or the end-of-file */
regular_char        [^\"\n\0\\]
string_literal      "{regular_char}|{escaped_char}"

operator            "{"|"}"|"("|")"|":"|";"|","|"+"|"-"|"*"|"/"|"^"|"."|"="|"<"|"<="|"<-"

%%
%{
    // Code run each time yylex is called.
    loc.step();
%}
    /* Rules */

    /* White spaces */
{white_space}           loc.step();
{single_line_comment}   loc.lines(0); loc.step();
\n+                     loc.lines(yyleng); loc.step();
<multi_line_comment>\n+ loc.lines(yyleng); loc.step();

<INITIAL>"(*" {
    started_comment = 1;
    comm_start_stack.push(loc);
    // As soon as (* is read, another lexical analyzer 
    // that only reads (* and *) is called  
    BEGIN(multi_line_comment);  
}

<INITIAL>"*)" { /* A comment must be open before being closed */
    print_error(loc.begin, "A comment must be open before being closed");
    return Parser::make_YYerror(loc);
}

<multi_line_comment>"(*" {
    comm_start_stack.push(loc);
    started_comment++;
}

<multi_line_comment>"*)" {
    /* Stack should not be empty */
    if(!comm_start_stack.empty()){
        comm_start_stack.pop();
    }
    started_comment--;
    if (started_comment == 0)
        BEGIN(INITIAL);     
}

    /* eat up multi-line comments */
<multi_line_comment>[^\0]  loc.step();  

<multi_line_comment><<EOF>> { /* All comment must be closed before the end of file */
    location comm_start = loc;
    /* Stack should not be empty */
    if(!comm_start_stack.empty()){
        comm_start = comm_start_stack.top();
    }
    /* Must empty stack for next comment*/
    while (!comm_start_stack.empty()) {
        comm_start_stack.pop();
    }
    print_error(comm_start.begin, "All comment must be closed before the end of file");
    BEGIN(INITIAL);
    return Parser::make_YYerror(comm_start);
}

{integer_literal} {  /* For integer literals, output the decimal value (whatever the input format was) */
    if(yytext[0] == '0' && yytext[1] == 'x')
        return Parser::make_INTEGER_LITERAL(std::stoi(yytext, nullptr, 16), loc);
    else
        return Parser::make_INTEGER_LITERAL(std::stoi(yytext), loc);
}

{integer_literal}{identifier} { /* An integer_literal cannot be directly followed by an identifier */
    print_error(loc.begin, "An integer_literal cannot be directly followed by an identifier");
    return Parser::make_YYerror(loc);
}

{type_identifier} { 
    return Parser::make_TYPE_IDENTIFIER(yytext, loc);
}

{object_identifier} {
    auto it = find(keywords.begin(), keywords.end(), yytext); /* Once an identifier/keyword is read, a table lookup decides if it is an identifier or a keyword */
    if(it != keywords.end())
        //return Parser::make_KEYWORDS(yytext, loc);
        switch(yytext[0]){
            case 'a': 
                if(strcmp( yytext, "and") == 0)
                    return Parser::make_AND(loc);
                break;
            case 'b': 
                if(strcmp( yytext, "bool") == 0)
                    return Parser::make_BOOL(loc);
                break;
            case 'c': 
                if(strcmp( yytext, "class") == 0)
                    return Parser::make_CLASS(loc);
                break;
            case 'd': 
                if(strcmp( yytext, "do") == 0)
                    return Parser::make_DO(loc);
                break;
            case 'e': 
                if(strcmp( yytext, "else") == 0)
                    return Parser::make_ELSE(loc);
                else if(strcmp( yytext, "extends") == 0)
                    return Parser::make_EXTENDS(loc);
                break;
            case 'f': 
                if(strcmp( yytext, "false") == 0)
                    return Parser::make_FALSE(loc);
                break;
            case 'i': 
                if(strcmp( yytext, "if") == 0)
                    return Parser::make_IF(loc);
                else if(strcmp( yytext, "in") == 0)
                    return Parser::make_IN(loc);
                else if(strcmp( yytext, "int32") == 0)
                    return Parser::make_INT32(loc);
                else if(strcmp( yytext, "isnull") == 0)
                    return Parser::make_ISNULL(loc);
                break;
            case 'l': 
                if(strcmp( yytext, "let") == 0)
                    return Parser::make_LET(loc);
                break;
            case 'n': 
                if(strcmp( yytext, "new") == 0)
                    return Parser::make_NEW(loc);
                else if(strcmp( yytext, "not") == 0)
                    return Parser::make_NOT(loc);
                break;
            case 's': 
                if(strcmp( yytext, "self") == 0)
                    return Parser::make_SELF(loc);
                else if(strcmp( yytext, "string") == 0)
                    return Parser::make_STRING(loc);
                break;
            case 't': 
                if(strcmp( yytext, "then") == 0)
                    return Parser::make_THEN(loc);
                else if(strcmp( yytext, "true") == 0)
                    return Parser::make_TRUE(loc);
                break;
            case 'u': 
                if(strcmp( yytext, "unit") == 0)
                    return Parser::make_UNIT(loc);
                break;
            case 'w': 
                if(strcmp( yytext, "while") == 0)
                    return Parser::make_WHILE(loc);
                break;
        }
    else
        return Parser::make_OBJECT_IDENTIFIER(yytext, loc);
}

\" {
    string_buffer = "";
    string_start = loc;
    BEGIN(STRING);
}

<STRING>{escaped_char} {
    int hex_value;
    std::string hex_string = "";
    switch(yytext[1]){
        case 'b': string_buffer += "\\x08"; break;
        case 't': string_buffer += "\\x09"; break;
        case 'n': string_buffer += "\\x0a"; break;
        case 'r': string_buffer += "\\x0d"; break;
        case '"': string_buffer += "\\x22"; break;
        case '\\': string_buffer += "\\x5c"; break;
        case 'x': 
            hex_string += yytext[2];
            hex_string += yytext[3];
            hex_value = stoi(hex_string, 0, 16);
            /* Replace non printable values */
            if((hex_value <= 0x1F && hex_value >= 0x00) || hex_value == 0x7F){
                string_buffer += yytext;
                break;
            }
            string_buffer += hex_value;
            break;
        case '\n': 
            loc.lines(1);
            BEGIN(NEW_LINE_STRING);
            break;
        default: 
            string_buffer += yytext[1]; 
            break;
    }
    loc.step();
} 

<STRING>{regular_char}+ {string_buffer += yytext;loc.step();}

<STRING>\\ { /* A backslash must be followed by escape_sequence */
    print_error(loc.begin, "A backslash must be followed by escape_sequence");
    return Parser::make_YYerror(loc);
}

<STRING>\n { /* Cannot use \n without backslash  */
    print_error(loc.begin, "Cannot use literal line feed without backslash");
    return Parser::make_YYerror(loc);
}

<STRING><<EOF>> { /* All string-literal must be closed before the end of file  */
    BEGIN(INITIAL);
    print_error(string_start.begin, "All string-literal must be closed before the end of file");
    return Parser::make_YYerror(string_start);
}

<STRING>\" {
    BEGIN(INITIAL);
    return Parser::make_STRING_LITERAL(string_buffer, string_start);
}

<NEW_LINE_STRING>{white_space} {
    loc.step();
    BEGIN(STRING);
}

<NEW_LINE_STRING>. {
    yyless(0);
    BEGIN(STRING);
}

<NEW_LINE_STRING><<EOF>> {
    BEGIN(INITIAL);
    print_error(string_start.begin, "All string-literal must be closed before the end of file");
    return Parser::make_YYerror(string_start);
}

{operator} {
    switch(yytext[0]){
        case '{': 
            return Parser::make_LBRACE(loc);
        case '}': 
            return Parser::make_RBRACE(loc);
        case '(': 
            return Parser::make_LPAR(loc);
        case ')': 
            return Parser::make_RPAR(loc);
        case ':': 
            return Parser::make_COLON(loc);
        case ';': 
            return Parser::make_SEMICOLON(loc);
        case ',': 
            return Parser::make_COMMA(loc);
        case '+': 
            return Parser::make_PLUS(loc);
        case '-': 
            return Parser::make_MINUS(loc);
        case '*': 
            return Parser::make_TIMES(loc);
        case '/': 
            return Parser::make_DIV(loc);
        case '^': 
            return Parser::make_POW(loc);
        case '.': 
            return Parser::make_DOT(loc);
        case '=': 
            return Parser::make_EQUAL(loc);
        case '<': {
            if(yytext[1] == '='){
                return Parser::make_LOWER_EQUAL(loc);
            }
            else if(yytext[1] == '-'){
                return Parser::make_ASSIGN(loc);
            }
            else{
                return Parser::make_LOWER(loc);
            }
            break;
        }
    }
}

. {
    print_error(loc.begin, "Illegal character in this context");
    return Parser::make_YYerror(loc);
}

<INITIAL><<EOF>> return Parser::make_YYEOF(loc);
%%

    /* User code */

static void print_error(const position &pos, const string &m)
{
    cerr << *(pos.filename) << ":"
         << pos.line << ":"
         << pos.column << ":"
         << " lexical error: "
         << m
         << endl;
}

void Driver::scan_begin()
{
    loc.initialize(&source_file);

    if (source_file.empty() || source_file == "-")
        yyin = stdin;
    else if (!(yyin = fopen(source_file.c_str(), "r")))
    {
        cerr << "cannot open " << source_file << ": " << strerror(errno) << '\n';
        exit(EXIT_FAILURE);
    }
}

void Driver::scan_end()
{
    fclose(yyin);
}