CC				= clang

CXX 			= clang++

LLVM_CONFIG		= llvm-config 

CXXFLAGS 		= -Wall -Wextra -ggdb3 $(shell ${LLVM_CONFIG} --cxxflags)

LDFLAGS			= $(shell ${LLVM_CONFIG} --ldflags --system-libs --libs all)

BISONFLAGS 		= -d -Wconflicts-sr -Wcounterexamples

EXEC			= vsopc

SRC				= main.cpp \
				  driver.cpp \
				  parser.cpp \
				  lexer.cpp \
				  print_visitor.cpp \
				  literals_visitor.cpp \
				  type.cpp \
				  check_classes.cpp \
				  post_sementic_analysis_visitor.cpp \
				  llvm.cpp \
				  code_generation_visitor.cpp 


OBJ	  			= $(SRC:.cpp=.o)


all: $(EXEC)

main.o: driver.hpp parser.hpp ast.hpp semantics_expressions.hpp type.hpp llvm.hpp code_generation_visitor.hpp

ast.o: ast.hpp

semantics_expressions.o: semantics_expressions.hpp

code_generation_visitor.o: code_generation_visitor.hpp

type.o: type.hpp

driver.o: driver.hpp parser.hpp

parser.o: driver.hpp parser.hpp

lexer.o: driver.hpp parser.hpp

llvm.o: llvm.hpp

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
	sudo cp -R ./runtime/ /usr/local/lib/vsop/
