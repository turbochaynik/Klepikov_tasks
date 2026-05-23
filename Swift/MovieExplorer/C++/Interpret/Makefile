CXX := g++
CXXFLAGS := -std=c++11 -g -o0 -Wall -Wextra -pedantic -O2

LEXER_SRC := lexer.cpp
LEXER_HEADER := lexer.h
SYNTAX_SRC := syntax.cpp
SYNTAX_HEADER := syntax.h
MAIN_SRC := main.cpp
POLIZ_SRC := poliz.cpp
POLIZ_HEADER := poliz.h
POLIZEX_HEADER := polizEx.h
EXECUTER_SRC := executer.cpp
EXECUTER_HEADER := executer.cpp

LEXER_OBJ := $(LEXER_SRC:.cpp=.o)
SYNTAX_OBJ := $(SYNTAX_SRC:.cpp=.o)
MAIN_OBJ := $(MAIN_SRC:.cpp=.o)
POLIZ_OBJ := $(POLIZ_SRC:.cpp=.o)
EXECUTER_OBJ := $(EXECUTER_SRC:.cpp=.o)
OBJS := $(LEXER_OBJ) $(SYNTAX_OBJ) $(MAIN_OBJ) $(POLIZ_OBJ) $(EXECUTER_OBJ)

TARGET := interpret

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@
%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

main.o: main.cpp $(LEXER_HEADER) $(SYNTAX_HEADER)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
