%{
    #include <iostream>
    #include <cerrno>
    #include <cstring>

    extern FILE* yyin;
    int yylex();

    void yyerror(const std::string msg){
        std::cerr << msg << std::endl;
    };
%}

%locations

%%

exp:

%%

int main( int argc, char **argv ){
    if(argc != 3){
        std::cerr << "expected format : ./vsopc -l <SOURCE-FILE>" << std::endl;
        return EXIT_FAILURE;
    }

    yyin = fopen( argv[2], "r");
    if(yyin == NULL){
        std::cerr << "cannot open " << argv[2] << ": " << std::strerror(errno) << std::endl;
        return EXIT_FAILURE;
    }

    yylex();
    
    fclose(yyin);
}
