%{
    #include <iostream>
    #include <cerrno>
    #include <cstring>

    extern FILE* yyin;
    char *file_name;
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

    file_name = argv[2];

    yylex();
    
    fclose(yyin);
}
