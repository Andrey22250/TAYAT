#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include "defines.h"

class TScaner {
private:
    char t[MAX_TEXT]; // исходный текст
    int uk;           // текуща€ позици€
    int line;           // номер строки
    int column;         // номер символа в строке
public:
    TScaner(const char* filename);
    ~TScaner() {}

    void PutUK(int i);
    int GetUK();
    void PrintError(const char* msg, const char* lex);
    inline void advance();
    int Scaner(TypeLex l);
    void GetData(const char* filename);

    // ¬озврат текущей позиции дл€ ошибок
    int GetLine() const { return line; }
    int GetColumn() const { return column; }
};

//ƒл€ тестов
//int main() {
//    int a = 10;
//    int b = 5;
//    float c = 3.14;
//    float d = 3.0;
//    float e = .5;
//    int result = a + b;
//    result = a - b;
//    result = a * b;
//    result = a / b;
//    result = a % b;
//    bool f, g = true;
//    bool g1 = false;
//    bool res = f && g;
//    res = f || g1;
//    res = !f;
//    a == b; a != b; a > b; a >= b; a <= b; a < b;
//    switch (a) {
//    case 1: b = 0; break;
//    case 2: b = 1; break;
//    default: b = 2;
//    }
//}