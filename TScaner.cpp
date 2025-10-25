#include "TScaner.h"

TScaner::TScaner(const char* filename) : uk(0), line(1), column(1) {
    GetData(filename);
}

void TScaner::PutUK(int i) { uk = i; }
int TScaner::GetUK() { return uk; }

// вывод ошибки
void TScaner::PrintError(const char* msg, const char* lex) {
    std::cerr << "Ошибка: " << msg << " возле '" << lex << "'\n";
}

// ввод данных
void TScaner::GetData(const char* filename) {
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Не удалось открыть файл: " << filename << "\n";
        t[0] = '\0';
        return;
    }

    // читаем весь файл в буфер t
    file.read(t, MAX_TEXT - 1);
    std::streamsize count = file.gcount();
    t[count] = '\0'; // добавляем терминатор строки

    uk = 0;
    line = 1;
    column = 1;
}

// --- Вспомогательные функции
static bool isIdentStart(char c) { return std::isalpha(c) || c == '_'; }
static bool isIdent(char c) { return std::isalnum(c) || c == '_'; }

inline void TScaner::advance() {
    if (t[uk] == '\n') {
        line++;
        column = 0;
    }
    else {
        column++;
    }
    uk++;
}

int TScaner::Scaner(TypeLex l) {
    int i = 0;
    l[0] = '\0';

    // Пропускаем пробелы и табуляции
    while (t[uk] && std::isspace((unsigned char)t[uk])) {
        advance();
    }

    if (t[uk] == '\0')
        return T_END;

    char c = t[uk];

    // --- Идентификаторы и ключевые слова ---
    if (isIdentStart(c)) {
        while (isIdent(t[uk])) {
            if (i < MAX_LEX - 1)
                l[i++] = t[uk];
            advance();
        }
        l[i] = '\0';

        if (strcmp(l, "main") == 0) return T_MAIN;
        if (strcmp(l, "int") == 0) return T_INT;
        if (strcmp(l, "short") == 0) return T_SHORT;
        if (strcmp(l, "long") == 0) return T_LONG;
        if (strcmp(l, "float") == 0) return T_FLOAT;
        if (strcmp(l, "bool") == 0) return T_BOOL;
        if (strcmp(l, "switch") == 0) return T_SWITCH;
        if (strcmp(l, "case") == 0) return T_CASE;
        if (strcmp(l, "break") == 0) return T_BREAK;
        if (strcmp(l, "default") == 0) return T_DEFAULT;
        if (strcmp(l, "true") == 0) return T_TRUE;
        if (strcmp(l, "false") == 0) return T_FALSE;

        return T_IDENTIF;
    }

    // --- Числа ---
    if (std::isdigit((unsigned char)c) || (c == '.' && std::isdigit((unsigned char)t[uk + 1]))) {
        char buf[MAX_LEX];
        int j = 0;
        bool isFloat = false;

        // 16-ричные
        if (t[uk] == '0' && (t[uk + 1] == 'x' || t[uk + 1] == 'X')) {
            buf[j++] = t[uk];
            advance();
            buf[j++] = t[uk];
            advance();

            bool anyHex = false;
            while (std::isxdigit((unsigned char)t[uk])) {
                anyHex = true;
                if (j < MAX_LEX - 1) buf[j++] = t[uk];
                advance();
            }
            buf[j] = '\0';

            if (!anyHex) {
                std::strncpy(l, buf, MAX_LEX - 1); l[MAX_LEX - 1] = 0;
                PrintError("Некорректный hex-литерал", l);
                return T_ERROR;
            }

            std::strncpy(l, buf, MAX_LEX - 1); l[MAX_LEX - 1] = 0;
            return T_CONST_HEX;
        }

        // Десятичные и вещественные
        if (t[uk] == '.') {
            isFloat = true;
            buf[j++] = t[uk];
            advance();
            while (std::isdigit((unsigned char)t[uk])) {
                if (j < MAX_LEX - 1) buf[j++] = t[uk];
                advance();
            }
        }
        else {
            while (std::isdigit((unsigned char)t[uk])) {
                if (j < MAX_LEX - 1) buf[j++] = t[uk];
                advance();
            }
            if (t[uk] == '.') {
                isFloat = true;
                if (j < MAX_LEX - 1) buf[j++] = t[uk];
                advance();
                while (std::isdigit((unsigned char)t[uk])) {
                    if (j < MAX_LEX - 1) buf[j++] = t[uk];
                    advance();
                }
            }
        }

        buf[j] = '\0';
        std::strncpy(l, buf, MAX_LEX - 1);
        l[MAX_LEX - 1] = '\0';

        return isFloat ? T_CONST_FLOAT : T_CONST_INT;
    }

    // --- Разделители и операторы ---
    switch (c) {
    case ',': strcpy(l, ","); advance(); return T_COMMA;
    case ';': strcpy(l, ";"); advance(); return T_SEMICOLON;
    case ':': strcpy(l, ":"); advance(); return T_COLON;
    case '(': strcpy(l, "("); advance(); return T_PAR_OPEN;
    case ')': strcpy(l, ")"); advance(); return T_PAR_CLOSED;
    case '{': strcpy(l, "{"); advance(); return T_BRACE_OPEN;
    case '}': strcpy(l, "}"); advance(); return T_BRACE_CLOSED;
    case '[': strcpy(l, "["); advance(); return T_BRACKET_OPEN;
    case ']': strcpy(l, "]"); advance(); return T_BRACKET_CLOSED;
    case '+': strcpy(l, "+"); advance(); return T_PLS;
    case '-': strcpy(l, "-"); advance(); return T_MNS;
    case '*': strcpy(l, "*"); advance(); return T_MULT;
    case '/':
        if (t[uk + 1] == '/') {
            advance(); advance();
            while (t[uk] && t[uk] != '\n') advance();
            return Scaner(l);
        }
        else {
            strcpy(l, "/");
            advance();
            return T_DIV;
        }
    case '%': strcpy(l, "%"); advance(); return T_MOD;
    case '!':
        if (t[uk + 1] == '=') { strcpy(l, "!="); advance(); advance(); return T_NEQ; }
        else { strcpy(l, "!"); advance(); return T_NOT; }
    case '=':
        if (t[uk + 1] == '=') { strcpy(l, "=="); advance(); advance(); return T_EQ; }
        else { strcpy(l, "="); advance(); return T_ASSIGN; }
    case '<':
        if (t[uk + 1] == '=') { strcpy(l, "<="); advance(); advance(); return T_LE; }
        else { strcpy(l, "<"); advance(); return T_LT; }
    case '>':
        if (t[uk + 1] == '=') { strcpy(l, ">="); advance(); advance(); return T_GE; }
        else { strcpy(l, ">"); advance(); return T_GT; }
    case '&':
        if (t[uk + 1] == '&') { strcpy(l, "&&"); advance(); advance(); return T_AND; }
        break;
    case '|':
        if (t[uk + 1] == '|') { strcpy(l, "||"); advance(); advance(); return T_OR; }
        break;
    }

    // --- Неизвестный символ ---
    l[0] = c; l[1] = '\0';
    PrintError("Неизвестный символ", l);
    advance();
    return T_ERROR;
}
