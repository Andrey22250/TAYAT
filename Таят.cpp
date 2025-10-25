#include <Windows.h>
#include "Parser.h"

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);
    
    Parser parser("input.txt");
    parser.parseAll();
}
