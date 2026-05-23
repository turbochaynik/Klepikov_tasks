#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>

enum class LexemeType {
    KEYWORD, IDENTIFIER, STR_LIT, INT_LIT, FLOAT_LIT,
    COMMA, SEMICOLON, FUNC, CALLED_FUNC,
    LBRACE, RBRACE, LBRACKET, RBRACKET,
    UNKNOWN, OPERATOR, OPERATOR_COMP,
    ENDING, LPAREN, RPAREN, INDENT
};

struct Lexeme {
    LexemeType type;
    std::string lex;
    int line;
    int column;
    Lexeme(LexemeType t, const std::string& v, int l, int c)
        : type(t), lex(v), line(l), column(c) {}
};

class Lexer {
public:
    explicit Lexer(const std::string& src);
    std::vector<Lexeme> lexemize();

private:
    std::string prog;
    size_t pos;
    int line;
    int column;

    static const std::vector<std::string> keywords;
    static const std::vector<std::string> called_funcs;
    
    void advance(int n = 1);
    void skipLineComment();
    void skipBlockComment();
    Lexeme readDigit();
    Lexeme readStr();
    bool handleOperators(std::vector<Lexeme>& lexemes);
    Lexeme readIdentifier();
    bool isKeyword(const std::string& str);
    bool isCalledFunc(const std::string& str);
};

#endif
