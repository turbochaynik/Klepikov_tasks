#include "lexer.h"
#include <cctype>
#include <algorithm>

const std::vector<std::string> Lexer::keywords = {
    "int", "float", "while", "if", "else",
    "return", "main", "print"
};

const std::vector<std::string> Lexer::called_funcs = {
    "my_id", "cur_turn", "players", "money",
    "raw", "production", "factories", "manufactured",
    "result_raw_sold", "result_raw_price", "result_prod_bought",
    "result_prod_price", "supply", "raw_price", "demand",
    "production_price", "buy", "sell", "prod", "turn", "active_players"
};

bool Lexer::isKeyword(const std::string& str) {
    return std::find(keywords.begin(), keywords.end(), str) != keywords.end();
}

bool Lexer::isCalledFunc(const std::string& str) {
    return std::find(called_funcs.begin(), called_funcs.end(), str) != called_funcs.end();
}

Lexer::Lexer(const std::string& src)
    : prog(src), pos(0), line(1), column(1) {}

std::vector<Lexeme> Lexer::lexemize() {
    std::vector<Lexeme> lexemes;
    bool at_line_start = true;
        size_t indent_count = 0;
    while (pos < prog.size()) {
        const char current = prog[pos];
    /*
        // Обработка пробелов в начале строки
        if (new_line && (current == ' ' || current == '\t')) {
            space_count = 0;
            const size_t start_line = line;
            const size_t start_col = column;
            
            while (pos < prog.size() && (prog[pos] == ' ' || prog[pos] == '\t')) {
                space_count += (prog[pos] == '\t') ? 4 : 1;
                advance();
            }
            
            if (space_count > 0) {
                lexemes.emplace_back(
                    LexemeType::INDENT,
                    std::to_string(space_count),
                    start_line,
                    start_col
                );
            }
            continue;
        }
        
        // Сброс флага новой строки при любом другом символе
        if (new_line) new_line = false;
        
        // Обработка перевода строки
        if (current == '\n') {
            new_line = true;
            advance();
            continue;
        }
        
        // Пропускаем пробелы не в начале строки
        if (isspace(current)) {
            advance();
            continue;
        }
     */
    if (at_line_start) {
                if (current == ' ' || current == '\t') {
                    indent_count = 0;
                    size_t start_line = line;
                    size_t start_col = column;
                    size_t start_pos = pos;

                    
                    // Считаем пробелы/табы
                    while (pos < prog.size() && (prog[pos] == ' ' || prog[pos] == '\t')) {
//                        indent_count += (prog[pos] == '\t') ? 4 : 1;
                        advance();
                    }
                    
                    // Создаем лексему с реальными пробелами (не количеством)
                    if (indent_count > 0) {
                        std::string indent_str = prog.substr(start_pos, pos - start_pos);
                        lexemes.emplace_back(
                            LexemeType::INDENT,
                            indent_str,  // Записываем реальные пробелы/табы
                            start_line,
                            start_col
                        );
                    }
                    continue;
                }
                at_line_start = false;
            }
            
            // Обработка перевода строки
            if (current == '\n') {
                at_line_start = true;
                advance();
                continue;
            }
            
            // Пропускаем пробелы не в начале строки
            if (isspace(current)) {
                advance();
                continue;
            }
        
        if (current == '/' && pos + 1 < prog.size()) {
            if (prog[pos+1] == '/') { skipLineComment(); continue; }
            if (prog[pos+1] == '*') { skipBlockComment(); continue; }
        }
        
        if (current == '"') {
            lexemes.push_back(readStr());
            continue;
        }
        
        if (isdigit(current)) {
            lexemes.push_back(readDigit());
            continue;
        }
        
        if (handleOperators(lexemes)) {
            continue;
        }
        
        if (isalpha(current) || current == '_') {
            lexemes.push_back(readIdentifier());
            continue;
        }
        
        lexemes.emplace_back(LexemeType::UNKNOWN, std::string(1, current), line, column);
        advance();
    }
    lexemes.emplace_back(LexemeType::ENDING, "", line, column);
    return lexemes;
}



void Lexer::advance(int n) {
    while (n-- > 0 && pos < prog.size()) {
        if (prog[pos] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        pos++;
    }
}

void Lexer::skipLineComment() {
    while (pos < prog.size() && prog[pos] != '\n') advance();
}
void Lexer::skipBlockComment() {
    advance(2);
    while (pos + 1 < prog.size() && !(prog[pos]=='*' && prog[pos+1]=='/')) advance();
    advance(2);
}
Lexeme Lexer::readDigit() {
    int start_line = line;
    int start_col = column;
    int start = pos;
    bool isfloat = false;

    if (pos < prog.size() && prog[pos] == '.') {
        isfloat = true;
        advance();
    }

    while (pos < prog.size() && (isdigit(prog[pos]) || prog[pos] == '.')) {
        if (prog[pos] == '.') {
            if (isfloat) break;
            isfloat = true;
        } else {
        }
        advance();
    }

    std::string lex = prog.substr(start, pos-start);
    return { isfloat ? LexemeType::FLOAT_LIT : LexemeType::INT_LIT, lex, start_line, start_col };
}

/*Lexeme Lexer::readStr() {
    int start_line = line;
    int start_col = column;
    advance();
    int start = pos;
    while (pos < prog.size() && prog[pos] != '"') advance();
    std::string lex = prog.substr(start, pos-start);
    advance();
    return { LexemeType::STR_LIT, lex, start_line, start_col };
}*/

Lexeme Lexer::readStr() {
    int start_line = line;
    int start_col = column;
    advance(); 
    std::string value;
    
    while (pos < prog.size() && prog[pos] != '"') {
        if (prog[pos] == '\\') {
            advance();
            if (pos >= prog.size()) break;
            
            switch (prog[pos]) {
                case 'n':  value += '\n'; break;
                case 't':  value += '\t'; break;
                case 'r':  value += '\r'; break;
                case '\\': value += '\\'; break;
                case '"':  value += '"';  break;
                default:   value += prog[pos]; break;
            }
            advance();
        } else {
            value += prog[pos];
            advance();
        }
    }
    
    advance();
    return { LexemeType::STR_LIT, value, start_line, start_col };
}

bool Lexer::handleOperators(std::vector<Lexeme>& lexemes) {
        int start_line = line;
    int start_col = column;
    
static const std::vector<std::pair<std::string, LexemeType>> ops = {
        {"&&", LexemeType::OPERATOR_COMP}, {"||", LexemeType::OPERATOR_COMP},
        {"<=", LexemeType::OPERATOR_COMP}, {">=", LexemeType::OPERATOR_COMP},
        {"!=", LexemeType::OPERATOR_COMP}, {"==", LexemeType::OPERATOR_COMP},
        {"+", LexemeType::OPERATOR},   {"-", LexemeType::OPERATOR},
        {"*", LexemeType::OPERATOR},   {"/", LexemeType::OPERATOR},
        {"=", LexemeType::OPERATOR},   {"<", LexemeType::OPERATOR_COMP},
        {">", LexemeType::OPERATOR_COMP},{"!", LexemeType::OPERATOR},
        {"%", LexemeType::OPERATOR},   {"&", LexemeType::OPERATOR},
        {";", LexemeType::SEMICOLON},  {",", LexemeType::COMMA},
        {"{", LexemeType::LBRACE},     {"}", LexemeType::RBRACE},
        {"(", LexemeType::LPAREN},     {")", LexemeType::RPAREN},
        {"[", LexemeType::LBRACKET},   {"]", LexemeType::RBRACKET}
    };

    for (const auto& op : ops) {
        if (prog.substr(pos, op.first.size()) == op.first) {
            lexemes.emplace_back(op.second, op.first, start_line, start_col);

            advance(op.first.size());
        return true;
        }
    }
    return false;
}


Lexeme Lexer::readIdentifier() {
    int start_line = line;
    int start_col = column;
    size_t start = pos;
    while (pos < prog.size() && (isalnum(prog[pos]) || prog[pos] == '_')) advance();
    std::string lex = prog.substr(start, pos - start);

    if (isKeyword(lex)) {
        return {LexemeType::KEYWORD, lex, start_line, start_col};
    }
    if (isCalledFunc(lex)) {
        return {LexemeType::CALLED_FUNC, lex, start_line, start_col};
    }
    return {LexemeType::IDENTIFIER, lex, start_line, start_col};
}
