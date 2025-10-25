#pragma once
#include <string>
#include <vector>
#include <iostream>

// Используем ваш defines.h для единообразия, но добавим свои типы
#include "defines.h" 

// Перечисление для семантических типов данных
enum DATA_TYPE {
    TYPE_UNKNOWN = 0,
    TYPE_INT,
    TYPE_SHORT_INT,
    TYPE_LONG_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_SCOPE // Специальный узел для обозначения области видимости
};

// Узел с информацией об идентификаторе
struct SemNode {
    std::string id;         // имя
    DATA_TYPE dataType;     // тип элемента
    std::vector<int> dimensions; // размеры массива (пуст для скаляров)
    int line;               // строка объявления
    int col;                // колонка объявления
};

// Класс для узла дерева (область видимости или переменная)
class Tree {
public:
    // Конструктор для корневого узла
    Tree();
    // Деструктор для очистки памяти
    ~Tree();

    void PrintTree() const;

    // --- Методы доступа (геттеры) ---
    SemNode* getNodeData() const { return n; }

    // Удобная обертка для получения типа
    DATA_TYPE getDataType() const {
        if (n) {
            return n->dataType;
        }
        return TYPE_UNKNOWN; // Или другое значение по умолчанию для узлов-областей
    }

    // Семантические действия
    void SemEnterBlock();
    void SemExitBlock();
    Tree* SemInclude(const std::string& id, DATA_TYPE type, const std::vector<int>& dims, int line, int col);
    Tree* SemGetId(const std::string& id, int line, int col);

    // Печать дерева для отладки
    void Print(int level = 0);

private:
    SemNode* n;         // данные (nullptr для узлов-областей)
    Tree* Up;           // родительская область
    Tree* Left;         // первый дочерний элемент/область
    Tree* Right;        // следующий элемент на том же уровне

    static Tree* Cur;   // указатель на текущую область видимости

    // Приватный конструктор для внутренних нужд
    Tree(Tree* up, SemNode* data = nullptr);

    // Вспомогательные функции поиска
    Tree* FindUp(const std::string& id);
    Tree* FindUpOneLevel(const std::string& id);
    Tree* DupControl(const std::string& id);

    void recursivePrint(const Tree* node, const std::string& prefix, bool isLast) const;
    std::string getDataTypeName(DATA_TYPE type) const;
    std::string getNodeInfo(const Tree* node) const;
};