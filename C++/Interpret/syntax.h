#ifndef SYNTAX_H
#define SYNTAX_H

#include "lexer.h"
#include <vector>
#include <stdexcept>
#include <string>
#include <iostream>
#include "poliz.h"
#include "executer.h"

#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <map>

class Parser {
private:
    std::stack<std::string> poliz_operators;
    void add_op(std::string op);
    void check_op(std::string op);
    void add_yes(std::string op);
    bool op_cmp(std::string op1, std::string op2);
    void while_not_LPAREN();

    std::unordered_map<std::string, std::pair<std::string, void*>> variables;
    std::unordered_map<std::string, int> massives;

	bool count_blocks_enabled = true;

	int block_count = 0;
	int current_depth = 0;
	int max_depth = 0;

    std::vector<Lexeme> lexemes;
    size_t position = 0;
    std::vector<std::unordered_set<std::string>> scopes;
        void enterScope();
        void exitScope();
        bool isDeclared(const std::string& name);
        void declare(const std::string& name); 
    
    std::unordered_map<std::string, std::string> type_table;

    const Lexeme& current() const;
    const Lexeme& peek(int n = 1) const;
    void advance(int n = 1);
    bool match(LexemeType type);
    bool match(const std::string& lex);
    void expect(LexemeType type);
    void expect(const std::string& lex);
    
    void parseWhileStatement();
    void parseFunctionIdentification();
    void parseParameterList();
    void parseReturnStatement();
    void parseStatement();
    void parseExpression();
    void parseIfStatement();
    void parseBlock();
    void parseFunctionCall();
    void parseAssignment();
    void parseGameCommand();
    void parsePrint();
    void parseTerm();
    void parseNewFunctionCall();
    void parseListOfValues();
    void parseVariable();
    void parseMassiveIdentification();
    void parseMassiveAssignment();
    void parseMain();
    void checkBooleanExpression(const std::vector<Lexeme>& expression);
    std::string parseExpressionWithType();
    std::string parseTermWithType();

public:
    List list;
    Parser(const std::vector<Lexeme>& lexemes);
    void parse();
};

std::string lexemeTypeToString(LexemeType type);

#endif
