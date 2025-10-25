#include "Parser.h"
#include <cstdio>

// Конструктор: создаёт TScaner и читает первый токен
Parser::Parser(const char* filename)
    : scaner(filename)
{
}

// Вспомогательные
void Parser::advance() {
    curTok = scaner.Scaner(curLex);
    // отладка:
    //std::cerr << "advance -> " << TokenName(curTok) << " '" << curLex << "'\n";
}

bool Parser::accept(int t) {
    if (curTok == t) { advance(); return true; }
    return false;
}

void Parser::match(int expected) {
    if (curTok == expected) {
        advance();
    }
    else {
        std::string s = "Ожидался " + std::string(TokenName(expected)) + " ("
            + TokenToString(expected) + ") "
            + ", найден " + std::string(TokenName(curTok))
            + " ('" + std::string(curLex) + "')";
        error(s);
    }
}

int Parser::peekNextToken(TypeLex outLex) {
    int saveUk = scaner.GetUK();
    // Для корректного получения следующего токена — просто вызвать Scaner (он использует uk)
    int nextTok = scaner.Scaner(outLex);
    // восстановить позицию
    scaner.PutUK(saveUk);
    // по восстановлению нужно обновить ничего — curTok/curLex остаются прежними
    return nextTok;
}

void Parser::error(const std::string& msg) {
    int line = scaner.GetLine();
    int col = scaner.GetColumn();
    std::string out = "Синтаксическая ошибка (" + std::to_string(line) +
        ", " + std::to_string(col) + ") :" + msg;
    throw std::runtime_error(out);
}

// Новая функция для семантических ошибок
void Parser::semantic_error(const std::string& msg) {
    int line = scaner.GetLine();
    int col = scaner.GetColumn();
    std::string out = "Семантическая ошибка (" + std::to_string(line) + ", " + std::to_string(col) + "): " + msg;
    throw std::runtime_error(out);
}

static std::string DataTypeName(DATA_TYPE t) {
    switch (t) {
    case TYPE_INT: return "int";
    case TYPE_SHORT_INT: return "short";
    case TYPE_LONG_INT: return "long";
    case TYPE_FLOAT: return "float";
    case TYPE_BOOL: return "bool";
    default: return "неизвестный тип";
    }
}

// Вход парсера
void Parser::parseAll() {
    try {
        parseProgram();
        if (curTok != T_END) {
            error("Лишние символы после конца программы: " + std::string(curLex));
        }
        std::cout << "Синтаксический анализ завершён успешно." << std::endl;
        std::cout << "Семантический анализ завершён успешно." << std::endl;
        semTree.PrintTree(); // <-- Раскомментируйте для отладочной печати дерева
    }
    catch (std::exception& ex) {
        std::cerr << ex.what() << std::endl;
    }
}

// <программа> -> { <описание> }*
void Parser::parseProgram() {
    advance();
    while (curTok != T_END) {
        // описание должно начинаться с типа (данные) или быть функцией (int main...)
        if (curTok == T_INT || curTok == T_SHORT || curTok == T_LONG || curTok == T_FLOAT || curTok == T_BOOL) {
            parseDescription();
        }
        else {
            // другие варианты не предусмотрены
            error("Ожидалось описание (тип или функция), найден " + std::string(TokenName(curTok)));
        }
    }
}

// <описание> -> <данные> | int main() { H }
void Parser::parseDescription() {
    // curTok сейчас — тип (T_INT / T_SHORT / ...)
    // Посмотрим на следующий токен
    TypeLex nextLex; nextLex[0] = '\0';
    int nextTok = peekNextToken(nextLex);

    // Если следующий токен — main (либо как ключевое слово, либо как идентификатор "main")
    if (nextTok == T_MAIN || (nextTok == T_IDENTIF && std::string(nextLex) == "main")) {
        // Сначала съедаем текущий токен-тип (int/short/...)
        if (!(curTok == T_INT || curTok == T_SHORT || curTok == T_LONG || curTok == T_FLOAT || curTok == T_BOOL)) {
            error("Ожидался тип перед функцией");
        }
        advance(); // теперь curTok == main (или T_MAIN)

        // Теперь съедаем лексему main (она у нас уже в curTok)
        if (curTok == T_MAIN || (curTok == T_IDENTIF && std::string(curLex) == "main")) {
            advance(); // теперь curTok == '('
        }
        else {
            error("Ожидалось 'main' после типа");
        }

        match(T_PAR_OPEN);
        match(T_PAR_CLOSED);
        match(T_BRACE_OPEN); semTree.SemEnterBlock();
        parseH(); semTree.SemExitBlock();
        match(T_BRACE_CLOSED);
        return;
    }

    // Иначе — это описание данных: просто вызываем parseData()
    // parseData ожидает, что curTok указывает на тип (он у нас пока не потреблён)
    parseData();
}

// D -> T L ;
void Parser::parseData() {
    DATA_TYPE currentType = parseType(); // Получаем семантический тип
    parseL(currentType);                 // Передаем его дальше
    match(T_SEMICOLON);
}

// <тип>
DATA_TYPE Parser::parseType() {
    DATA_TYPE type = TYPE_UNKNOWN;
    if (curTok == T_INT) type = TYPE_INT;
    else if (curTok == T_SHORT) type = TYPE_SHORT_INT;
    else if (curTok == T_LONG) type = TYPE_LONG_INT;
    else if (curTok == T_FLOAT) type = TYPE_FLOAT;
    else if (curTok == T_BOOL) type = TYPE_BOOL;
    else {
        error("Ожидался тип (int/short/long/float/bool)");
    }
    advance();
    return type;
}

// L -> V { , V }
void Parser::parseL(DATA_TYPE type) {
    parseV(type);
    while (curTok == T_COMMA) {
        advance();
        parseV(type);
    }
}

// V -> i <spiski indeksov> | B
void Parser::parseV(DATA_TYPE type) {
    if (curTok != T_IDENTIF) error("Ожидался идентификатор");

    std::string name = curLex;
    int line = scaner.GetLine(), col = scaner.GetColumn();
    advance();

    std::vector<int> dims;
    parseDeclarationIndices(dims);

    try {
        semTree.SemInclude(name, type, dims, line, col);
    }
    catch (const std::exception& e) {
        semantic_error(e.what());
    }

    if (accept(T_ASSIGN)) {
        DATA_TYPE rightType = parseE();
        checkAssignmentCompatibility(type, rightType);
    }
}

// B -> i = E (вспомогательная)
void Parser::parseB() {
    expectIdentifier();
    match(T_ASSIGN);
    parseE();
}

// indices -> { '[' E ']' }*
void Parser::parseIndices(std::vector<int>& dims) {
    while (accept(T_BRACKET_OPEN)) {
        DATA_TYPE indexType = parseE();
        if (!isIntegerType(indexType)) {
            semantic_error("Индекс массива должен быть целочисленным.");
        }
        dims.push_back(1); // Просто для подсчета измерений
        match(T_BRACKET_CLOSED);
    }
}

void Parser::expectIdentifier() {
    if (curTok == T_IDENTIF) {
        advance();
    }
    else error("Ожидался идентификатор");
}

void Parser::expectConstant() {
    if (curTok == T_CONST_INT || curTok == T_CONST_HEX || curTok == T_CONST_FLOAT || curTok == T_TRUE || curTok == T_FALSE) {
        advance();
    }
    else error("Ожидалась константа");
}

// H -> sequence of descriptions or operators (может быть ε)
void Parser::parseH() {
    while (curTok != T_BRACE_CLOSED && curTok != T_END) {
        if (curTok == T_INT || curTok == T_SHORT || curTok == T_LONG || curTok == T_FLOAT || curTok == T_BOOL) {
            parseData();
        }
        else {
            parseO();
        }
    }
}

// O -> ; | { H } | F | B;
void Parser::parseO() {
    // 1. Проверка на пустой оператор
    if (accept(T_SEMICOLON)) {
        return;
    }

    // 2. Проверка на блок
    if (curTok == T_BRACE_OPEN) {
        match(T_BRACE_OPEN);
        semTree.SemEnterBlock();
        parseH();
        semTree.SemExitBlock();
        match(T_BRACE_CLOSED);
        return;
    }

    // 3. Проверка на switch
    if (curTok == T_SWITCH) {
        parseF();
        return;
    }

    // 4. Проверка на break
    if (accept(T_BREAK)) {
        match(T_SEMICOLON);
        return;
    }

    // 5. Проверка на оператор присваивания (B;) - ключевое изменение
    if (curTok == T_IDENTIF) {
        std::string varName = curLex;
        Tree* leftNode = nullptr;
        try {
            leftNode = semTree.SemGetId(varName, scaner.GetLine(), scaner.GetColumn());
        }
        catch (const std::exception& e) {
            semantic_error(e.what());
        }
        advance();

        SemNode* nodeData = leftNode->getNodeData();
        bool isDeclaredAsArray = !nodeData->dimensions.empty();

        int usedIndices = parseUsageIndices();

        if (isDeclaredAsArray) {
            if (usedIndices == 0) {
                semantic_error("Нельзя присвоить значение целому массиву '" + varName + "'. Укажите индекс.");
            }
            if (usedIndices != nodeData->dimensions.size()) {
                std::string msg = "Неверное количество индексов для массива '" + varName +
                    "'. Ожидалось " + std::to_string(nodeData->dimensions.size()) +
                    ", а получено " + std::to_string(usedIndices) + ".";
                semantic_error(msg);
            }
        }
        else {
            if (usedIndices > 0) {
                semantic_error("Попытка индексации переменной '" + varName + "', не являющейся массивом.");
            }
        }

        match(T_ASSIGN);
        DATA_TYPE rightType = parseE();
        checkAssignmentCompatibility(leftNode->getDataType(), rightType);

        match(T_SEMICOLON);
        return;
    }

    // Если мы дошли до сюда, значит, оператор нам неизвестен
    error("Неизвестный оператор начался с " + std::string(TokenName(curTok)));
}

// F -> switch ( E ) { <список ветвей> <ветвь default> }
void Parser::parseF() {
    match(T_SWITCH);
    match(T_PAR_OPEN);
    DATA_TYPE switchExprType = parseE();
    if (!isIntegerType(switchExprType)) {
        semantic_error("Выражение в switch должно иметь целочисленный тип.");
    }
    match(T_PAR_CLOSED);
    match(T_BRACE_OPEN);

    // Главный цикл разбора тела switch.
    // Работает, пока мы не дойдем до конца блока.
    while (curTok != T_BRACE_CLOSED && curTok != T_END) {
        if (curTok == T_CASE) {
            parseCaseBranch();
        }
        else if (curTok == T_DEFAULT) {
            // Разбор ветки default
            advance(); // съедаем 'default'
            match(T_COLON);
            // Разбираем операторы до следующей ветки или конца switch
            while (curTok != T_CASE && curTok != T_DEFAULT && curTok != T_BRACE_CLOSED && curTok != T_END) {
                parseO();
            }
        }
        else {
            // Внутри switch могут быть только case или default ветки.
            error("Ожидалась ветка 'case' или 'default'");
        }
    }

    match(T_BRACE_CLOSED);
}

// case <10-ричная> : <список операторов> ;
void Parser::parseCaseBranch() {
    match(T_CASE);
    // допускаем только десятичную целую константу (в лексере она помечена T_CONST_INT)
    if (curTok == T_CONST_INT) {
        advance();
    }
    else {
        error("Ожидалась целая константа после case");
    }
    match(T_COLON);
    // список операторов
    while (curTok != T_CASE && curTok != T_DEFAULT && curTok != T_BRACE_CLOSED && curTok != T_END) {
        if (curTok == T_BREAK) { advance(); match(T_SEMICOLON); }
        else
        {
            parseO();
        }
    }
}

// ----------------- Выражения ------------------
// E -> C { || C }
DATA_TYPE Parser::parseE() {
    DATA_TYPE leftType = parseC();
    if (accept(T_OR)) {
        checkLogicalCompatibility(leftType);
        DATA_TYPE rightType = parseC();
        checkLogicalCompatibility(rightType);
        return TYPE_BOOL;
    }
    return leftType;
}

// C -> R { && R }
DATA_TYPE Parser::parseC() {
    DATA_TYPE leftType = parseR();
    if (accept(T_AND)) {
        checkLogicalCompatibility(leftType);
        DATA_TYPE rightType = parseR();
        checkLogicalCompatibility(rightType);
        return TYPE_BOOL;
    }
    return leftType;
}

// R -> Q { (== | !=) Q }
DATA_TYPE Parser::parseR() {
    DATA_TYPE leftType = parseQ();
    if (curTok == T_EQ || curTok == T_NEQ) {
        advance();
        DATA_TYPE rightType = parseQ();
        checkEqualityCompatibility(leftType, rightType);
        return TYPE_BOOL;
    }
    return leftType;
}

// Q -> A { < | > | <= | >= A }
DATA_TYPE Parser::parseQ() {
    DATA_TYPE leftType = parseA();
    if (curTok == T_LT || curTok == T_GT || curTok == T_LE || curTok == T_GE) {
        advance();
        DATA_TYPE rightType = parseA();
        checkRelationalCompatibility(leftType, rightType);
        return TYPE_BOOL; // Результат сравнения всегда bool
    }
    return leftType; // Если операции не было, просто пробрасываем тип наверх
}

// A -> M { + | - M }
DATA_TYPE Parser::parseA() {
    DATA_TYPE leftType = parseM();
    while (curTok == T_PLS || curTok == T_MNS) {
        advance();
        DATA_TYPE rightType = parseM();
        if (!isNumericType(leftType) || !isNumericType(rightType)) {
            semantic_error("Операнды операций '+' и '-' должны быть числовыми.");
        }
        if (leftType == TYPE_FLOAT || rightType == TYPE_FLOAT) {
            leftType = TYPE_FLOAT;
        }
    }
    return leftType;
}

// M -> U { * | / | % U }
DATA_TYPE Parser::parseM() {
    DATA_TYPE leftType = parseU();
    while (curTok == T_MULT || curTok == T_DIV || curTok == T_MOD) {
        int op = curTok;
        advance();
        DATA_TYPE rightType = parseU();

        if (op == T_MOD) {
            if (!isIntegerType(leftType) || !isIntegerType(rightType)) {
                semantic_error("Операнды операции '%' должны быть целочисленными.");
            }
        }
        else {
            if (!isNumericType(leftType) || !isNumericType(rightType)) {
                semantic_error("Операнды операций '*', '/' должны быть числовыми.");
            }
        }
        // Правило продвижения типов: int * float -> float
        if (leftType == TYPE_FLOAT || rightType == TYPE_FLOAT) {
            leftType = TYPE_FLOAT;
        }
    }
    return leftType;
}

// U -> !U | +U | -U | P
DATA_TYPE Parser::parseU() {
    if (accept(T_NOT)) {
        DATA_TYPE type = parseU();
        checkLogicalCompatibility(type); // '!' применим только к bool
        return TYPE_BOOL;
    }
    if (accept(T_PLS) || accept(T_MNS)) {
        DATA_TYPE type = parseU();
        if (!isNumericType(type)) {
            semantic_error("Унарные '+' и '-' применимы только к числовым типам.");
        }
        return type;
    }
    return parseP();
}

// P -> i | c | ( E ) | i <indices>
DATA_TYPE Parser::parseP() {
    if (curTok == T_IDENTIF) {
        std::string name = curLex;
        Tree* node = nullptr;
        try {
            node = semTree.SemGetId(name, scaner.GetLine(), scaner.GetColumn());
        }
        catch (const std::exception& e) {
            semantic_error(e.what());
        }
        advance();

        SemNode* nodeData = node->getNodeData();
        bool isDeclaredAsArray = !nodeData->dimensions.empty();

        int usedIndices = parseUsageIndices();

        if (isDeclaredAsArray) {
            if (usedIndices == 0) {
                semantic_error("Массив '" + name + "' не может быть использован в выражении без указания индекса.");
            }
            if (usedIndices != nodeData->dimensions.size()) {
                std::string msg = "Неверное количество индексов для массива '" + name +
                    "'. Ожидалось " + std::to_string(nodeData->dimensions.size()) +
                    ", а получено " + std::to_string(usedIndices) + ".";
                semantic_error(msg);
            }
        }
        else {
            if (usedIndices > 0) {
                semantic_error("Попытка индексации переменной '" + name + "', не являющейся массивом.");
            }
        }

        return node->getDataType();
    }

    if (curTok == T_CONST_INT || curTok == T_CONST_HEX) {
        advance(); return TYPE_INT;
    }
    if (curTok == T_CONST_FLOAT) {
        advance(); return TYPE_FLOAT;
    }
    if (curTok == T_TRUE || curTok == T_FALSE) {
        advance(); return TYPE_BOOL;
    }

    if (accept(T_PAR_OPEN)) {
        DATA_TYPE exprType = parseE();
        match(T_PAR_CLOSED);
        return exprType;
    }

    error("Ожидался операнд (идентификатор, константа или (E))");
    return TYPE_UNKNOWN; // Никогда не будет достигнуто
}

// --- Вспомогательные семантические функции ---

// Проверяет, является ли тип целочисленным
bool Parser::isIntegerType(DATA_TYPE t) const {
    return t == TYPE_INT || t == TYPE_SHORT_INT || t == TYPE_LONG_INT;
}

// Проверяет, является ли тип числовым (целым или вещественным)
bool Parser::isNumericType(DATA_TYPE t) const {
    return isIntegerType(t) || t == TYPE_FLOAT;
}

// Правило 3: Проверка совместимости при присваивании
void Parser::checkAssignmentCompatibility(DATA_TYPE left, DATA_TYPE right) {
    bool isLeftInt = isIntegerType(left);
    bool isRightInt = isIntegerType(right);

    if (isLeftInt && isRightInt) return;
    if (left == TYPE_FLOAT && right == TYPE_FLOAT) return;
    if (left == TYPE_BOOL && right == TYPE_BOOL) return;

    // --- УЛУЧШЕННОЕ СООБЩЕНИЕ ОБ ОШИБКЕ ---
    std::string msg = "Несовместимые типы при присваивании: невозможно присвоить '" +
        DataTypeName(right) + "' переменной типа '" + DataTypeName(left) + "'.";
    semantic_error(msg);
}

// Правило 4: Проверка типа для логических операций
void Parser::checkLogicalCompatibility(DATA_TYPE type) {
    if (type != TYPE_BOOL) {
        std::string msg = "Операнд логической операции должен иметь тип bool, а не '" +
            DataTypeName(type) + "'.";
        semantic_error(msg);
    }
}

// Правило 5: Проверка для <, <=, >, >=
void Parser::checkRelationalCompatibility(DATA_TYPE left, DATA_TYPE right) {
    if (!isNumericType(left) || !isNumericType(right)) {
        semantic_error("Операнды операций сравнения (<, >, <=, >=) должны быть числовыми.");
    }
}

// Правило 5: Проверка для ==, !=
void Parser::checkEqualityCompatibility(DATA_TYPE left, DATA_TYPE right) {
    if (left != right) {
        std::string msg = "Операнды операций равенства (==, !=) должны быть одного типа, а не '" +
            DataTypeName(left) + "' и '" + DataTypeName(right) + "'.";
        semantic_error(msg);
    }
}

void Parser::parseDeclarationIndices(std::vector<int>& dims) {
    while (accept(T_BRACKET_OPEN)) {
        // В реальном компиляторе здесь бы вычисляли константное выражение,
        // чтобы получить размер (например, 100).
        // Мы пока просто разбираем выражение и запоминаем, что измерение есть.
        parseE();
        dims.push_back(1); // Запоминаем, что есть одно измерение
        match(T_BRACKET_CLOSED);
    }
}

int Parser::parseUsageIndices() {
    int count = 0;
    while (accept(T_BRACKET_OPEN)) {
        count++;
        DATA_TYPE indexType = parseE();
        if (!isIntegerType(indexType)) {
            semantic_error("Индекс массива должен быть целочисленным, а не '" + DataTypeName(indexType) + "'.");
        }
        match(T_BRACKET_CLOSED);
    }
    return count;
}