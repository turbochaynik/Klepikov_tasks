#include "syntax.h"
using namespace std;
Parser::Parser(const std::vector<Lexeme>& lexemes) : lexemes(lexemes) {}


void Parser::add_yes(std::string op) {
    // std::cout << "Added: " + op << std::endl;
    if (op == "+") list.Add(new PolizFunPlus);
    else if (op == "-")list.Add(new PolizFunMinus);
    else if (op == "*")list.Add(new PolizFunMul);
    else if (op == "/")list.Add(new PolizFunDiv);
    else if (op == "%")list.Add(new PolizFunMod);
    else if (op == "&&")list.Add(new PolizFunAnd);
    else if (op == "||")list.Add(new PolizFunOr);
    else if (op == "=") list.Add(new PolizAssign);
    else if (op == "<")list.Add(new PolizFunLess);
    else if (op == "<=")list.Add(new PolizFunLessEqual);
    else if (op == ">")list.Add(new PolizFunGreater);
    else if (op == ">=")list.Add(new PolizFunGreaterEqual);
    else if (op == "==")list.Add(new PolizFunEqual);
    else if (op == "!=") list.Add(new PolizFunNotEqual);
}


bool Parser::op_cmp(std::string op1, std::string op2) {
    static std::map<std::string, int> priority = {
        {"!", 7},
        {"*", 6},
        {"/", 6},
        {"%", 6},
        {"+", 5},
        {"-", 5},
        {"<", 4},
        {"<=", 4},
        {">", 4},
        {">=", 4},
        {"==", 3},
        {"!=", 3},
        {"&&", 2},
        {"||", 1},
        {"=", 0}
    };

    if (priority.find(op1) == priority.end() || priority.find(op2) == priority.end()) {
        throw std::runtime_error("Unknown operator in op_cmp: " + op1 + " or " + op2);
    }

    return priority[op1] > priority[op2];
}


void Parser::while_not_LPAREN(){
    std::string tmp;
        while ((tmp = poliz_operators.top()) != "("){
            poliz_operators.pop();
            add_yes(tmp);
        }
        poliz_operators.pop();
}


void Parser::add_op(std::string op){
    //std::cout << "Coming: " + op << std::endl;
    if (op == "(") poliz_operators.push("(");
    else if (op == ")") {
        while_not_LPAREN();
    }
    else if (poliz_operators.top() == "(" or op_cmp(op, poliz_operators.top())) {
        poliz_operators.push(op);
    }
    else {
        std::string top_tmp;
        while (true){
            top_tmp = poliz_operators.top();
            if (top_tmp != "(" && !op_cmp(op, top_tmp)) {
                poliz_operators.pop();
                add_yes(top_tmp);
            }
            else {
                poliz_operators.push(op);
                break;
            }
        }
    }
}


void Parser::enterScope() {
    scopes.emplace_back();
}

void Parser::exitScope() {
    if (!scopes.empty()) {
        scopes.pop_back();
    }
}

bool Parser::isDeclared(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        if (it->count(name)) return true;
    }
    return false;
}

void Parser::declare(const std::string& name) {
    if (scopes.empty()) enterScope();
    if (scopes.back().count(name)) {
        throw std::runtime_error("Variable '" + name + "' already declared in this scope.");
    }
    scopes.back().insert(name);
}



const Lexeme& Parser::current() const {
    if (position >= lexemes.size())
        throw std::runtime_error("Unexpected end of input");
    return lexemes[position];
}

const Lexeme& Parser::peek(int n) const {
    if (position + n >= lexemes.size())
        throw std::runtime_error("Unexpected end of input");
    return lexemes[position + n];
}

void Parser::advance(int n) {
    position += n;
    if (position >= lexemes.size()) throw std::runtime_error("Unexpected end of input");
}

bool Parser::match(LexemeType type) {
    if (current().type == type) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(const std::string& lex) {
    if (current().lex == lex) {
        advance();
        return true;
    }
    return false;
}

void Parser::expect(LexemeType type) {
    if (!match(type)) {
        throw std::runtime_error("Expected " + lexemeTypeToString(type) +
        " at line " + std::to_string(current().line) +
        ", column " + std::to_string(current().column));
}
}

void Parser::expect(const std::string& lex) {
    if (!match(lex)) {
        throw std::runtime_error("Expected " + lex +
                                 " at line " + std::to_string(current().line) +
                                 ", column " + std::to_string(current().column));
    }
}


std::string lexemeTypeToString(LexemeType type) {
    switch (type) {
        case LexemeType::KEYWORD:       return "KEYWORD";
        case LexemeType::IDENTIFIER:    return "IDENTIFIER";
        case LexemeType::STR_LIT:       return "STR_LIT";
        case LexemeType::INT_LIT:       return "INT_LIT";
        case LexemeType::FLOAT_LIT:     return "FLOAT_LIT";
        case LexemeType::COMMA:         return "COMMA";
        case LexemeType::SEMICOLON:     return "SEMICOLON";
        case LexemeType::FUNC:          return "FUNC";
        case LexemeType::CALLED_FUNC:   return "CALLED_FUNC";
        case LexemeType::LBRACE:        return "LBRACE";
        case LexemeType::RBRACE:        return "RBRACE";
        case LexemeType::LBRACKET:      return "LBRACKET";
        case LexemeType::RBRACKET:      return "RBRACKET";
        case LexemeType::OPERATOR:      return "OPERATOR";
        case LexemeType::OPERATOR_COMP: return "OPERATOR_COMP";
        case LexemeType::LPAREN:        return "LPAREN";
        case LexemeType::RPAREN:        return "RPAREN";
        case LexemeType::ENDING:        return "ENDING";
        default:                        return "UNKNOWN";
    }
}




void Parser::parse() {
    enterScope();
        while (position < lexemes.size() && current().type != LexemeType::ENDING) {
            parseStatement();
        }
	std::cout << "[Blocks] Всего блоков: " << block_count
                  << ", максимальная глубина: " << max_depth << std::endl;

    exitScope();
}

void Parser::parseStatement() {
    if (current().type == LexemeType::KEYWORD && current().lex == "if") {
        advance();
        parseIfStatement();
    }
    else if (current().type == LexemeType::KEYWORD && current().lex == "while") {
        advance();
        parseWhileStatement();
    }
    else if (current().type == LexemeType::KEYWORD && current().lex == "return") {
        advance();
        parseReturnStatement();
    }
    else if (current().type == LexemeType::KEYWORD && current().lex == "print") {
        advance();
        parsePrint();
    }
    else if (current().type == LexemeType::IDENTIFIER && peek().lex == "=") {
        parseAssignment();
    }
    else if (current().type == LexemeType::IDENTIFIER && peek().type == LexemeType::LBRACKET) {
        parseMassiveAssignment();
    }
   else if (current().type == LexemeType::KEYWORD &&
         (current().lex == "int" || current().lex == "float") &&
         (peek().type == LexemeType::IDENTIFIER || peek().type == LexemeType::KEYWORD) &&
         peek(2).type == LexemeType::LPAREN) {
    parseFunctionIdentification();
}

    else if (current().type == LexemeType::KEYWORD &&
             (current().lex == "int" || current().lex == "float") &&
             peek().type == LexemeType::IDENTIFIER &&
             peek(2).type == LexemeType::LBRACKET) {
        parseMassiveIdentification();
    }
    else if (current().type == LexemeType::KEYWORD &&
             (current().lex == "int" || current().lex == "float") &&
             peek().type == LexemeType::IDENTIFIER) {
        parseVariable();
    }
    else if (current().type == LexemeType::CALLED_FUNC &&
             (current().lex == "buy" || current().lex == "sell" ||
              current().lex == "prod" || current().lex == "build" ||
              current().lex == "endturn" || current().lex == "turn")) {
        parseGameCommand();
    }
    else if (current().type == LexemeType::CALLED_FUNC) {
        parseFunctionCall();
    }
    else if (current().type == LexemeType::IDENTIFIER && peek().type == LexemeType::LPAREN) {
        parseNewFunctionCall();
        expect(";");
    }
    else if (current().type == LexemeType::ENDING) {
         throw std::runtime_error("Lost }");
    }
    else if (current().type == LexemeType::LBRACE) {
        parseBlock();
    }
    else {
        throw std::runtime_error("Unexpected statement at line " +
            std::to_string(current().line) +
            ", column " + std::to_string(current().column));
    }
}

void Parser::parseAssignment() {
    std::string varName = current().lex;
    if (!isDeclared(varName)) {
        throw std::runtime_error("Undeclared variable '" + varName + "'");
    }
    std::string lhs_type = type_table[varName];

    expect(LexemeType::IDENTIFIER);
    expect("=");
    list.Add(new PolizVarAddr(variables[varName].second));
    // std::string rhs_type = parseExpressionWithType();
    parseExpression();
    list.Add(new PolizAssign());
    // if (lhs_type != rhs_type) {
    //     throw std::runtime_error("Type mismatch in assignment to '" + varName +
    //         "': cannot assign " + rhs_type + " to " + lhs_type);
    // }

    expect(LexemeType::SEMICOLON);
}

void Parser::parseMassiveAssignment() {
    std::string name = current().lex;

    if (!isDeclared(name)) {
        throw std::runtime_error("Undeclared array '" + name + "' at line " +
            std::to_string(current().line) + ", column " + std::to_string(current().column));
    }

    expect(LexemeType::IDENTIFIER);
    expect(LexemeType::LBRACKET);
    parseExpression();
    expect(LexemeType::RBRACKET);
    expect("=");
    parseExpression();
    expect(LexemeType::SEMICOLON);
}


void Parser::parseReturnStatement(){
    parseExpression();
    expect(LexemeType::SEMICOLON);
}


void Parser::parseExpression() {
    poliz_operators.push("(");
    parseTerm();
    while(current().type == LexemeType::OPERATOR) {
        if (match("+")){
            add_op("+");
            parseTerm();
        }
        else if (match("-")){
            add_op("-");
            parseTerm();
        }
        else if (match("*")){
            add_op("*");
            parseTerm();
        }
        else if (match("/")){
            add_op("/");
            parseTerm();
        }
        else if (match("%")){
            add_op("%");
            parseTerm();
        }
        else if (match("!")){
            add_op("!");
            parseTerm();
        }
    }
    if (current().type == LexemeType::OPERATOR_COMP) {
        if (match("<")){
            add_op("<");
            parseTerm();
        }
        else if (match("<=")){
            add_op("<=");
            parseTerm();
        }
        else if (match(">")){
            add_op(">");
            parseTerm();
        }
        else if (match(">=")){
            add_op(">=");
            parseTerm();
        }
        else if (match("==")){
            add_op("==");
            parseTerm();
        }
        else if (match("&&")){
            add_op("&&");
            parseTerm();
        }
        else if (match("||")){
            add_op("||");
            parseTerm();
        }
        else if (match("!=")){
            add_op("!=");
            parseTerm();
        }
    }
    while_not_LPAREN();
}


void Parser::parseTerm() {

        // std::cout << "Parce Term" << std::endl;
    if (current().type == LexemeType::INT_LIT || current().type == LexemeType::FLOAT_LIT) {
        if (current().type == LexemeType::INT_LIT){
            list.Add(new PolizInt(std::stoi(current().lex)));
            // std::cout << "Added: " << current().lex << std::endl;
        }
        else if (current().type == LexemeType::FLOAT_LIT){
            list.Add(new PolizFloat(std::stof(current().lex)));
        }
        advance();
    }
    else if (current().type == LexemeType::IDENTIFIER && peek().type == LexemeType::LBRACKET) {

        // std::cout << "Elem massive: " + current().lex << std::endl;
        if (!isDeclared(current().lex)) {
            throw std::runtime_error("Undeclared array '" + current().lex +
                                     "' at line " + std::to_string(current().line) +
                                     ", column " + std::to_string(current().column));
        }
        int max_size = massives[current().lex];
        auto inf = variables[current().lex + "[0]"];
        advance(2);
        parseExpression();
        //list.Add(new PolizMassive(inf.first, &(inf.second), max_size));
        expect(LexemeType::RBRACKET);
    }
    else if (current().type == LexemeType::IDENTIFIER && peek().type == LexemeType::LPAREN) {
        if (!isDeclared(current().lex)) {
            throw std::runtime_error("Undeclared function '" + current().lex +
                                     "' at line " + std::to_string(current().line) +
                                     ", column " + std::to_string(current().column));
        }
        parseNewFunctionCall();
    }
    else if (current().type == LexemeType::IDENTIFIER) {
        if (!isDeclared(current().lex)) {
            throw std::runtime_error("Undeclared identifier '" + current().lex +
                                     "' at line " + std::to_string(current().line) +
                                     ", column " + std::to_string(current().column));
        }
        auto info = variables[current().lex];

        std::string var_type = info.first;
        void* var_p = info.second;
        
        list.Add(new PolizVar(var_type, var_p));

        // std::cout << "Added: " << current().lex << std::endl;
        advance();
    }
    else if (current().type == LexemeType::CALLED_FUNC) {
        parseFunctionCall();
    }
    else {
        expect(LexemeType::LPAREN);
        add_op("(");
        parseExpression();
        expect(LexemeType::RPAREN);
        add_op(")");
    }
}


void Parser::parseFunctionIdentification() {
    std::string name = peek(1).lex;
    declare(name);
    advance(3);
    enterScope();
    parseParameterList();
    expect(LexemeType::RPAREN);
    if (name == "main") {
        expect(LexemeType::LBRACE);

        count_blocks_enabled = false;

        while (current().type != LexemeType::RBRACE) {
            parseStatement();
        }

        expect(LexemeType::RBRACE);
        count_blocks_enabled = true;
    } else {
        parseBlock();
    }

    exitScope();
}


void Parser::parseParameterList() {
    while(current().type != LexemeType::RPAREN){
        if (current().type == LexemeType::KEYWORD &&
            (current().lex == "int" || current().lex == "float")) {
            
            std::string type = current().lex;
            advance();

            if (current().type != LexemeType::IDENTIFIER) {
                throw std::runtime_error("Expected identifier for parameter");
            }

            std::string param_name = current().lex;
            declare(param_name);
            type_table[param_name] = type;
            advance();

            if (current().type == LexemeType::LBRACKET) {
                advance();
                expect("0");
                expect(LexemeType::RBRACKET);
            }

            if (current().type != LexemeType::RPAREN) {
                expect(LexemeType::COMMA);
            }
        }
        else {
            throw std::runtime_error("Invalid parameter declaration at line " +
                std::to_string(current().line));
        }
    }
}



void Parser::parseIfStatement() {
    expect(LexemeType::LPAREN);
    // size_t start = position;
    parseExpression();
    // size_t end = position;
    expect(LexemeType::RPAREN);
    // std::vector<Lexeme> condition(lexemes.begin() + start, lexemes.begin() + end);
    // checkBooleanExpression(condition);

    PolizLabel* false_label = new PolizLabel(nullptr);
    list.Add(false_label);
    list.Add(new PolizOpGoFalse());
    parseBlock();

    if (match("else")) {
        PolizLabel* end_label = new PolizLabel(nullptr);
        list.Add(end_label);
        list.Add(new PolizOpGo());
        false_label->Set(list.Get_End());
        parseBlock();
        end_label->Set(list.Get_End());
    } 
    else {
        false_label->Set(list.Get_End());
    }
}


void Parser::parseWhileStatement() {
    expect(LexemeType::LPAREN);
    // size_t start = position;
    // parseExpressionWithType();
    // size_t end = position;
    PolizLabel* start_while = new PolizLabel(list.Get_End());
    parseExpression();
    expect(LexemeType::RPAREN);
    PolizLabel* false_label = new PolizLabel(nullptr);
    list.Add(false_label);
    list.Add(new PolizOpGoFalse());

    // std::vector<Lexeme> condition(lexemes.begin() + start, lexemes.begin() + end);
    // checkBooleanExpression(condition);
    parseBlock();
    list.Add(start_while);
    list.Add(new PolizOpGo);
    false_label->Set(list.Get_End());
    // if (match("else")) {
    //     parseBlock();
    // }
}


void Parser::parsePrint() {
    int n = 0;
    while (current().type != LexemeType::SEMICOLON){

        if (current().type == LexemeType::CALLED_FUNC) {
            parseFunctionCall();
        }
        else if (current().type == LexemeType::IDENTIFIER && peek().type == LexemeType::LPAREN){
            parseNewFunctionCall();
        }
        // else if (current().type == LexemeType::IDENTIFIER && peek().type == LexemeType::LBRACKET) {
        //     parseMassive();
        // }
        else if (current().type == LexemeType::IDENTIFIER) {
            
            // std::cout << "Parcing massive" << std::endl;
            parseExpression();
            // std::cout << "Ending Parcing massive" << std::endl;
        }
        else if (current().type != LexemeType::STR_LIT){
            parseExpression();
        }
        else {
            list.Add(new PolizString(current().lex));
            advance();
        }
        n++;
        if (current().type != LexemeType::SEMICOLON){
            expect(LexemeType::COMMA);
        }
    }
    list.Add(new PolizInt(n));
    list.Add(new PolizPrint);
    expect(LexemeType::SEMICOLON);
}


void Parser::parseNewFunctionCall(){
    advance(2);
    parseListOfValues();
    expect(LexemeType::RPAREN);
}

void Parser::parseListOfValues() {
    if (current().type != LexemeType::RPAREN) {
        parseExpression();
    }
    if (current().type != LexemeType::RPAREN) {
        expect(LexemeType::COMMA);
        parseListOfValues();
    }
}

void Parser::parseBlock() {
    expect(LexemeType::LBRACE);

    if (count_blocks_enabled) {
        block_count++;
        current_depth++;
        if (current_depth > max_depth)
            max_depth = current_depth;
    }

    enterScope();

    while (current().type != LexemeType::RBRACE) {
        if (current().type == LexemeType::ENDING) {
            throw std::runtime_error("Unexpected end of input: missing closing '}' at block ending");
        }
	    parseStatement();
    }

    expect(LexemeType::RBRACE);
    exitScope();

    if (count_blocks_enabled) {
        current_depth--;
    }
}

void Parser::parseFunctionCall() {
    expect(LexemeType::CALLED_FUNC);
    expect(LexemeType::LPAREN);
    while (current().type != LexemeType::RPAREN) {
        parseExpression();
        if (!match(LexemeType::COMMA) && current().type != LexemeType::RPAREN) {
            throw std::runtime_error("Expected ',' or ')' in function call at line " +
                                   std::to_string(current().line) +
                                   ", column " + std::to_string(current().column));
        }
    }
    advance();
}

void Parser::parseGameCommand() {
    if (current().lex == "buy") {
        advance();
        
        parseExpression();
        
        expect(LexemeType::COMMA);
        parseExpression();
        list.Add(new PolizBuy);
    }
    else if (current().lex == "sell") {
        advance();
        
        parseExpression();
        
        expect(LexemeType::COMMA);
        parseExpression();
        list.Add(new PolizSell);
    }
    else if (current().lex == "prod") {
        advance();
        parseExpression();
        list.Add(new PolizProd);
    }
    else if (current().lex == "build") {
        advance();
        parseExpression();
        list.Add(new PolizBuild);
    }
    else if (current().lex == "endturn" || current().lex == "turn") {
        advance();
        list.Add(new PolizEndturn);
    }
    
    expect(LexemeType::SEMICOLON);
}

void Parser::parseVariable() {
    std::string type = current().lex;
    std::string name = peek().lex;
    declare(name);
    type_table[name] = type;

    void* pointer = nullptr;
    if (type == "int") {
        pointer = new int(0);
    }
    else if (type == "float") {
        pointer = new float(0.0);
    }
    variables[name] = {type, pointer};
    list.Add(new PolizVarAddr(pointer));


    advance(2);
    expect(LexemeType::SEMICOLON);
}




void Parser::parseMassiveIdentification() {
    std::string type = current().lex;
    std::string name = peek().lex;
    declare(name);
    advance(3);
    //parseExpression();
    if (current().type == LexemeType::INT_LIT){
        int n = stoi(current().lex);
        void** pointers = new void*[n];
        // if (type == "int") int** pointers = new int*[n];
        // else /*if (type == "float")*/ float** pointers = new float*[n];
        massives[name] = n;
        for (int i = 0; i < n; ++i){
            if (type == "int") {
                pointers[i] = new int(0);
            }
            else if (type == "float") {
                pointers[i] = new float(0.0);
            }
            variables[name + "[" + to_string(i) + "]"] = {type, pointers[i]};

        list.Add(new PolizVarAddr(pointers[i]));
        }
    }
    expect(LexemeType::INT_LIT);
    expect(LexemeType::RBRACKET);
    expect(LexemeType::SEMICOLON);
}




void Parser::parseMain() {
    expect("int");
    expect("main");
    expect("(");
    expect(")");
    parseBlock();
}


std::string Parser::parseExpressionWithType() {
    std::string left = parseTermWithType();

    if (current().type == LexemeType::OPERATOR_COMP) {
        std::string op = current().lex;
        advance();
        std::string right = parseExpressionWithType();

        if ((left != "int" && left != "float") || (right != "int" && right != "float")) {
            throw std::runtime_error("Operands of comparison '" + op + "' must be numeric (int or float)");
        }


        return "int";
    }

    if (current().type == LexemeType::OPERATOR) {
        advance();
        std::string right = parseExpressionWithType();

        if (left == "float" || right == "float") return "float";
        return "int";
    }

    return left;
}

std::string Parser::parseTermWithType() {
    if (current().type == LexemeType::INT_LIT) {
        list.Add(new PolizInt(std::stoi(current().lex)));
        advance();
        return "int";
    }

    if (current().type == LexemeType::FLOAT_LIT) {
        list.Add(new PolizFloat(std::stof(current().lex)));
        advance();
        return "float";
    }

    if (current().type == LexemeType::IDENTIFIER && peek().type == LexemeType::LBRACKET) {
        std::string name = current().lex;
        if (!isDeclared(name)) {
            throw std::runtime_error("Undeclared array '" + name + "'");
        }

        advance();
        expect(LexemeType::LBRACKET);

        std::string index_type = parseExpressionWithType();
        if (index_type != "int") {
            throw std::runtime_error("Array index for '" + name + "' must be integer");
        }

        expect(LexemeType::RBRACKET);
        return type_table[name];
    }

    if (current().type == LexemeType::IDENTIFIER) {
        std::string name = current().lex;

        if (!isDeclared(name)) {
            throw std::runtime_error("Undeclared identifier '" + name + "'");
        }

        if (peek().type == LexemeType::LPAREN) {
            parseNewFunctionCall();
            return "int";
        }

        advance();
        return type_table[name];
    }

    if (current().type == LexemeType::CALLED_FUNC) {
        parseFunctionCall();
        return "int";
    }

    if (current().type == LexemeType::LPAREN) {
        advance();
        std::string type = parseExpressionWithType();
        expect(LexemeType::RPAREN);
        return type;
    }

    throw std::runtime_error("Unexpected term in expression at line " + std::to_string(current().line));
}



void Parser::checkBooleanExpression(const std::vector<Lexeme>& expression) {
    if (expression.size() == 1 &&
            (expression[0].type == LexemeType::INT_LIT && expression[0].lex == "1")) {
            return; 
        }
    bool hasLogic = false;
    for (const auto& lex : expression) {
        if (lex.type == LexemeType::OPERATOR_COMP || (lex.lex == "&&" || lex.lex == "||" || lex.lex == "!")) {
            hasLogic = true;
            break;
        }
    }
    if (!hasLogic) {
        throw std::runtime_error("Condition must be a boolean expression (use comparison or logical operators)");
    }
}
