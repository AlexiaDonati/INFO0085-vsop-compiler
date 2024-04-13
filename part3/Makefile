CXX 			= clang++

CXXFLAGS 		= -Wall -Wextra -ggdb3

BISONFLAGS 		= -d -Wconflicts-sr -Wcounterexamples

EXEC			= vsopc

SRC				= main.cpp \
				  driver.cpp \
				  parser.cpp \
				  lexer.cpp \
				  print_visitor.cpp \
				  literals_visitor.cpp

OBJ	  			= $(SRC:.cpp=.o)


all: $(EXEC)

main.o: driver.hpp parser.hpp ast.hpp semantics_expressions.hpp

ast.o: ast.hpp

semantics_expressions.o: semantics_expressions.hpp

driver.o: driver.hpp parser.hpp

parser.o: driver.hpp parser.hpp

lexer.o: driver.hpp parser.hpp

$(EXEC): $(OBJ)
	$(CXX) -o $@ $(LDFLAGS) $(OBJ)

parser.cpp: parser.y
	bison $(BISONFLAGS) -o parser.cpp $^

parser.hpp: parser.y
	bison $(BISONFLAGS) -o parser.cpp $^

lexer.cpp: lexer.lex
	flex $(LEXFLAGS) -o lexer.cpp $^

clean:
	@rm -f $(EXEC)
	@rm -f $(OBJ)
	@rm -f lexer.cpp
	@rm -f parser.cpp parser.hpp location.hh

.PHONY: clean

install-tools:
	sudo apt-get install flex bison
