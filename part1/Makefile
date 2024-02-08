# Flex and Bison Compiler Flags
FLEX = flex
BISON = bison
BISONFLAGS = -d

# Executable name
EXEC = vsopc

# Flex and Bison Source Files
LEX_SOURCE = lexer.lex
BISON_SOURCE = parser.y

# C Compiler
CC = clang++
CFLAGS = -Wall -Wextra

# Generated C Source Files
LEX_OUTPUT = lex.yy.c
BISON_OUTPUT = parser.tab.c
BISON_HEADER = parser.tab.h

# Object Files
OBJECTS = $(LEX_OUTPUT:.c=.o) $(BISON_OUTPUT:.c=.o)

# Targets
all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) -o $(EXEC)

$(BISON_OUTPUT): $(BISON_SOURCE)
	$(BISON) $(BISONFLAGS) $(BISON_SOURCE)

$(LEX_OUTPUT): $(LEX_SOURCE) $(BISON_OUTPUT)
	$(FLEX) $(LEX_SOURCE)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install-tools:
	sudo apt-get install flex
	sudo apt-get install bison	

clean:
	rm -f $(EXEC) $(OBJECTS) $(LEX_OUTPUT) $(BISON_OUTPUT) $(BISON_HEADER)

.PHONY: clean
