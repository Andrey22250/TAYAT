#include "SemTree.h"
#include <stdexcept>

// Инициализация статического указателя
Tree* Tree::Cur = nullptr;

// Конструктор корневого узла
Tree::Tree() : n(nullptr), Up(nullptr), Left(nullptr), Right(nullptr) {
    Cur = this; // Корень становится текущей областью
}

// Приватный конструктор
Tree::Tree(Tree* up, SemNode* data) : n(data), Up(up), Left(nullptr), Right(nullptr) {}

// Деструктор для рекурсивного удаления
Tree::~Tree() {
    delete n;
    delete Left;
    delete Right;
}

// Вход в новый блок
void Tree::SemEnterBlock() {
    Tree* newScope = new Tree(Cur); // Создаем новую область с указателем на текущую

    // Привязываем новую область как потомка текущей
    if (Cur->Left == nullptr) {
        Cur->Left = newScope;
    }
    else {
        Tree* temp = Cur->Left;
        while (temp->Right != nullptr) {
            temp = temp->Right;
        }
        temp->Right = newScope;
    }
    Cur = newScope; // Новая область становится текущей
}

// Выход из блока
void Tree::SemExitBlock() {
    if (Cur->Up != nullptr) {
        Cur = Cur->Up;
    }
}

// Проверка на дубликат на текущем уровне
Tree* Tree::DupControl(const std::string& id) {
    return Cur->FindUpOneLevel(id);
}

// Включение нового идентификатора
Tree* Tree::SemInclude(const std::string& id, DATA_TYPE type, const std::vector<int>& dims, int line, int col) {
    Tree* duplicateNode = DupControl(id);
    if (duplicateNode != nullptr) {
        // --- УЛУЧШЕННОЕ СООБЩЕНИЕ ОБ ОШИБКЕ ---
        SemNode* originalNodeData = duplicateNode->getNodeData();
        std::string msg = "Повторное объявление идентификатора '" + id + "'.";
        msg += " Предыдущее объявление было на строке " + std::to_string(originalNodeData->line);
        msg += ", позиция " + std::to_string(originalNodeData->col);

        throw std::runtime_error(msg);
    }

    SemNode* newNodeData = new SemNode{ id, type, dims, line, col };
    Tree* newNode = new Tree(Cur, newNodeData);

    if (Cur->Left == nullptr) {
        Cur->Left = newNode;
    }
    else {
        Tree* temp = Cur->Left;
        while (temp->Right != nullptr) {
            temp = temp->Right;
        }
        temp->Right = newNode;
    }
    return newNode;
}


// Поиск идентификатора во всех видимых областях
Tree* Tree::SemGetId(const std::string& id, int line, int col) {
    Tree* found = FindUp(id);
    if (found == nullptr) {
        std::string msg = "Идентификатор '" + id + "' не объявлен";
        throw std::runtime_error(msg);
    }
    return found;
}

// Поиск вверх по областям
Tree* Tree::FindUp(const std::string& id) {
    for (Tree* p = Cur; p != nullptr; p = p->Up) {
        Tree* found = p->FindUpOneLevel(id);
        if (found) return found;
    }
    return nullptr;
}

// Поиск только на одном уровне
Tree* Tree::FindUpOneLevel(const std::string& id) {
    for (Tree* p = this->Left; p != nullptr; p = p->Right) {
        // Проверяем только узлы с данными (переменные), а не вложенные области
        if (p->n != nullptr && p->n->id == id) {
            return p;
        }
    }
    return nullptr;
}

// Отладочная печать
void Tree::Print(int level) {
    for (int i = 0; i < level; ++i) std::cout << "  ";
    if (n) {
        std::cout << "ID: " << n->id << ", Type: " << n->dataType;
        if (!n->dimensions.empty()) {
            std::cout << " [Array]";
        }
        std::cout << std::endl;
    }
    else {
        std::cout << "SCOPE" << std::endl;
    }

    if (Left) Left->Print(level + 1);
    if (Right) Right->Print(level);
}

// --- Вспомогательная функция для получения имени типа ---
std::string Tree::getDataTypeName(DATA_TYPE type) const {
    switch (type) {
    case TYPE_INT: return "INT";
    case TYPE_SHORT_INT: return "SHORT_INT";
    case TYPE_LONG_INT: return "LONG_INT";
    case TYPE_FLOAT: return "FLOAT";
    case TYPE_BOOL: return "BOOL";
    default: return "UNKNOWN";
    }
}

// --- Вспомогательная функция для форматирования информации об узле ---
std::string Tree::getNodeInfo(const Tree* node) const {
    if (node->n == nullptr) {
        return "SCOPE";
    }

    std::string info = "ID: " + node->n->id + ", Type: " + getDataTypeName(node->n->dataType);
    if (!node->n->dimensions.empty()) {
        info += ", Array Dims: [" + std::to_string(node->n->dimensions.size()) + "]";
    }
    return info;
}

// --- Публичный метод-обертка для запуска печати ---
void Tree::PrintTree() const {
    // Печатаем информацию о корневом (глобальном) узле
    std::cout << "\n--- Визуализация семантического дерева ---\n";
    std::cout << getNodeInfo(this) << " (Global)" << std::endl;

    // Запускаем рекурсивную печать для всех его прямых потомков
    Tree* child = this->Left;
    while (child != nullptr) {
        recursivePrint(child, "", (child->Right == nullptr));
        child = child->Right;
    }
    std::cout << "-------------------------------------\n";
}

// --- Рекурсивная функция-отрисовщик ---
void Tree::recursivePrint(const Tree* node, const std::string& prefix, bool isLast) const {
    if (node == nullptr) return;

    // 1. Печатаем префикс и информацию о текущем узле
    std::cout << prefix;
    std::cout << (isLast ? "-- " : "|-- ");
    std::cout << getNodeInfo(node) << std::endl;

    // 2. Готовим новый префикс для дочерних узлов
    std::string newPrefix = prefix + (isLast ? "    " : "|   ");

    // 3. Рекурсивно вызываем для всех дочерних узлов
    Tree* child = node->Left;
    while (child != nullptr) {
        // Последний ли дочерний узел в списке? Проверяем по child->Right
        recursivePrint(child, newPrefix, (child->Right == nullptr));
        child = child->Right;
    }
}