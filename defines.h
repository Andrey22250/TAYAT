#pragma once

#define MAX_TEXT 10000
#define MAX_LEX 10

// Лексемы (ключевые слова)
#define T_MAIN      1
#define T_INT       2
#define T_SHORT     3
#define T_LONG      4
#define T_FLOAT     5
#define T_BOOL      6
#define T_SWITCH    7
#define T_CASE      8
#define T_BREAK     9
#define T_DEFAULT   10
#define T_TRUE      11
#define T_FALSE     12

// Идентификаторы
#define T_IDENTIF   20

// Константы
#define T_CONST_INT   30
#define T_CONST_HEX   31
#define T_CONST_FLOAT 32
#define T_CONST_BOOL  33

// Разделители
#define T_COMMA        40
#define T_SEMICOLON    41
#define T_COLON        42
#define T_PAR_OPEN     43
#define T_PAR_CLOSED   44
#define T_BRACE_OPEN   45
#define T_BRACE_CLOSED 46
#define T_BRACKET_OPEN 47
#define T_BRACKET_CLOSED 48

// Операторы
#define T_OR      50
#define T_AND     51
#define T_LT      52
#define T_LE      53
#define T_GT      54
#define T_GE      55
#define T_EQ      56
#define T_NEQ     57
#define T_PLS     58
#define T_MNS     59
#define T_MULT    60
#define T_DIV     61
#define T_MOD     62
#define T_ASSIGN  63
#define T_NOT     64

// Прочее
#define T_END     100
#define T_ERROR   200

typedef char TypeLex[MAX_LEX];

inline const char* TokenName(int type) {
    switch (type) {
    case T_MAIN: return "T_MAIN";
    case T_INT: return "T_INT";
    case T_SHORT: return "T_SHORT";
    case T_LONG: return "T_LONG";
    case T_FLOAT: return "T_FLOAT";
    case T_BOOL: return "T_BOOL";
    case T_SWITCH: return "T_SWITCH";
    case T_CASE: return "T_CASE";
    case T_BREAK: return "T_BREAK";
    case T_DEFAULT: return "T_DEFAULT";
    case T_TRUE: return "T_TRUE";
    case T_FALSE: return "T_FALSE";

    case T_IDENTIF: return "T_IDENTIF";
    case T_CONST_INT: return "T_CONST_INT";
    case T_CONST_HEX: return "T_CONST_HEX";
    case T_CONST_FLOAT: return "T_CONST_FLOAT";
    case T_CONST_BOOL: return "T_CONST_BOOL";

    case T_COMMA: return "T_COMMA";
    case T_SEMICOLON: return "T_SEMICOLON";
    case T_COLON: return "T_COLON";
    case T_PAR_OPEN: return "T_PAR_OPEN";
    case T_PAR_CLOSED: return "T_PAR_CLOSED";
    case T_BRACE_OPEN: return "T_BRACE_OPEN";
    case T_BRACE_CLOSED: return "T_BRACE_CLOSED";
    case T_BRACKET_OPEN: return "T_BRACKET_OPEN";
    case T_BRACKET_CLOSED: return "T_BRACKET_CLOSED";

    case T_OR: return "T_OR";
    case T_AND: return "T_AND";
    case T_LT: return "T_LT";
    case T_LE: return "T_LE";
    case T_GT: return "T_GT";
    case T_GE: return "T_GE";
    case T_EQ: return "T_EQ";
    case T_NEQ: return "T_NEQ";
    case T_PLS: return "T_PLS";
    case T_MNS: return "T_MNS";
    case T_MULT: return "T_MULT";
    case T_DIV: return "T_DIV";
    case T_MOD: return "T_MOD";
    case T_ASSIGN: return "T_ASSIGN";
    case T_NOT: return "T_NOT";

    case T_END: return "T_END";
    case T_ERROR: return "T_ERROR";
    default: return "UNKNOWN";
    }
}

inline const char* TokenToString(int type) {
    switch (type) {
        
    case T_MAIN: return "'main'";
    case T_INT: return "'int'";
    case T_SHORT: return "'short'";
    case T_LONG: return "'long'";
    case T_FLOAT: return "'float'";
    case T_BOOL: return "'bool'";
    case T_SWITCH: return "'switch'";
    case T_CASE: return "'case'";
    case T_BREAK: return "'break'";
    case T_DEFAULT: return "'default'";
    case T_TRUE: return "'true'";
    case T_FALSE: return "'false'";

    case T_IDENTIF: return "идентификатор";
    case T_CONST_INT: return "целая константа";
    case T_CONST_HEX: return "шестнадцатеричная константа";
    case T_CONST_FLOAT: return "вещественная константа";
    case T_CONST_BOOL: return "логическая константа";

    case T_COMMA: return "','";
    case T_SEMICOLON: return "';'";
    case T_COLON: return "':'";
    case T_PAR_OPEN: return "'('";
    case T_PAR_CLOSED: return "')'";
    case T_BRACE_OPEN: return "'{'";
    case T_BRACE_CLOSED: return "'}'";
    case T_BRACKET_OPEN: return "'['";
    case T_BRACKET_CLOSED: return "']'";

    case T_OR: return "'||'";
    case T_AND: return "'&&'";
    case T_LT: return "'<'";
    case T_LE: return "'<='";
    case T_GT: return "'>'";
    case T_GE: return "'>='";
    case T_EQ: return "'=='";
    case T_NEQ: return "'!='";
    case T_PLS: return "'+'";
    case T_MNS: return "'-'";
    case T_MULT: return "'*'";
    case T_DIV: return "'/'";
    case T_MOD: return "'%'";
    case T_ASSIGN: return "'='";
    case T_NOT: return "'!'";

    case T_END: return "конец файла";
    case T_ERROR: return "ошибка лексемы";
    default: return "неизвестный токен";
    }
}