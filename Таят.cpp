#include <Windows.h>
#include "Parser.h"

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    const char* filename = "input.txt"; // фиксированный файл
    try {
        Parser parser(filename);
        parser.parseAll();
    }
    catch (std::exception& ex) {
        std::cerr << "Ошибка инициализации парсера: " << ex.what() << std::endl;
    }
    return 0;
}