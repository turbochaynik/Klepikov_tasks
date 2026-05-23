#include <iostream>
#include <fstream>
#include "lexer.h"
#include "syntax.h"
#include "poliz.h"
#include "executer.h"
#include <string>
#include <vector>
#include <unordered_map>

using namespace std;
void printLexemes(const std::vector<Lexeme>& lexemes) {
    for (const auto& lx : lexemes) {
        std::cout << "Line " << lx.line << ":" << lx.column << "\t";
        switch (lx.type) {
            case LexemeType::KEYWORD:       std::cout << "KEYWORD"; break;
            case LexemeType::IDENTIFIER:    std::cout << "IDENTIFIER"; break;
            case LexemeType::STR_LIT:       std::cout << "STR_LIT"; break;
            case LexemeType::INT_LIT:       std::cout << "INT_LIT"; break;
            case LexemeType::FLOAT_LIT:     std::cout << "FLOAT_LIT"; break;
            case LexemeType::COMMA:         std::cout << "COMMA"; break;
            case LexemeType::SEMICOLON:     std::cout << "SEMICOLON"; break;
            case LexemeType::FUNC:          std::cout << "FUNC"; break;
            case LexemeType::CALLED_FUNC:   std::cout << "CALLED_FUNC"; break;
            case LexemeType::LBRACE:        std::cout << "LBRACE"; break;
            case LexemeType::RBRACE:        std::cout << "RBRACE"; break;
            case LexemeType::LPAREN:        std::cout << "LPAREN"; break;
            case LexemeType::RPAREN:        std::cout << "RPAREN"; break;
            case LexemeType::LBRACKET:      std::cout << "LBRACKET"; break;
            case LexemeType::RBRACKET:      std::cout << "RBRACKET"; break;
            case LexemeType::OPERATOR:      std::cout << "OPERATOR"; break;
            case LexemeType::OPERATOR_COMP: std::cout << "OPERATOR_COMP"; break;
            case LexemeType::ENDING:        std::cout << "ENDING"; break;
            default:                        std::cout << "UNKNOWN"; break;
        }
        std::cout << "\t" << lx.lex << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <source-file>\n";
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file) { std::cerr << "Cannot open file\n"; return 1; }
    std::string content((std::istreambuf_iterator<char>(file)), {});

    Lexer lexer(content);
    auto lexemes = lexer.lexemize();
    std::cout << "=== Lexemes ===\n";
    printLexemes(lexemes);
    Parser parser(lexemes);
    try {
            parser.parse();
            std::cout << "Parsing completed successfully." << std::endl;
            std::cout << "Semantic analyzing completed successfully." << std::endl;

            Executer exec(parser.list);
            exec.Execute();
	    return 0;
        }
        catch (const std::runtime_error& e) {
            std::cerr << "[Semantic Error] " << e.what() << std::endl;
            return 1;
        }
        catch (...) {
            std::cerr << "Unknown error occurred" << std::endl;
            return 2;
        }
    cout << "Parsing completed successfully." << endl;


/*  
    ----------------------------------------------------------------
    ДОПОЛНИТЕЛЬНОЕ ЗАДАНИЕ (НАЙТИ НАИБОЛЕЕ ЧАСТО ВСТРЕЧАЕМУЮ ЛЕКСЕМУ И КОЛИЧЕСТВО ВСТРЕЧ)
    ----------------------------------------------------------------

    std::unordered_map<std::string, int> freq;
    for (auto& lx : lexemes) {
	if (lx.type == LexemeType::ENDING || lx.type == LexemeType::UNKNOWN || lx.lex.empty()) {
		continue;}
	++freq[lx.lex];
    }

    std::string mostCommon;
    int maxCount = 0;
    for (auto& [lexeme, count] : freq) {
	if (count > maxCount) {
	    maxCount = count;
	    mostCommon = lexeme;
	}
    }

    if (maxCount > 0) {
	std::cout << "\n LEXEME : " << mostCommon << "\n NUMBER : " << maxCount <<std::endl;
}
*/
    return 0;
}
