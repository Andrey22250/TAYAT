#include <Windows.h>
#include "TScaner.h"

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    std::string input;
    const char* filename = "input.txt";
    TScaner scaner(filename);
    TypeLex l;
    int type;

    do {
        type = scaner.Scaner(l);
        std::cout << TokenName(type) << " ---> " << l << std::endl;
    } while (type != T_END);

    return 0;
}