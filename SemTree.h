#pragma once
#include <string>
#include <vector>
#include <iostream>

// ���������� ��� defines.h ��� ������������, �� ������� ���� ����
#include "defines.h" 

// ������������ ��� ������������� ����� ������
enum DATA_TYPE {
    TYPE_UNKNOWN = 0,
    TYPE_INT,
    TYPE_SHORT_INT,
    TYPE_LONG_INT,
    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_SCOPE // ����������� ���� ��� ����������� ������� ���������
};

// ���� � ����������� �� ��������������
struct SemNode {
    std::string id;         // ���
    DATA_TYPE dataType;     // ��� ��������
    std::vector<int> dimensions; // ������� ������� (���� ��� ��������)
    int line;               // ������ ����������
    int col;                // ������� ����������
};

// ����� ��� ���� ������ (������� ��������� ��� ����������)
class Tree {
public:
    // ����������� ��� ��������� ����
    Tree();
    // ���������� ��� ������� ������
    ~Tree();

    void PrintTree() const;

    // --- ������ ������� (�������) ---
    SemNode* getNodeData() const { return n; }

    // ������� ������� ��� ��������� ����
    DATA_TYPE getDataType() const {
        if (n) {
            return n->dataType;
        }
        return TYPE_UNKNOWN; // ��� ������ �������� �� ��������� ��� �����-��������
    }

    // ������������� ��������
    void SemEnterBlock();
    void SemExitBlock();
    Tree* SemInclude(const std::string& id, DATA_TYPE type, const std::vector<int>& dims, int line, int col);
    Tree* SemGetId(const std::string& id, int line, int col);

    // ������ ������ ��� �������
    void Print(int level = 0);

private:
    SemNode* n;         // ������ (nullptr ��� �����-��������)
    Tree* Up;           // ������������ �������
    Tree* Left;         // ������ �������� �������/�������
    Tree* Right;        // ��������� ������� �� ��� �� ������

    static Tree* Cur;   // ��������� �� ������� ������� ���������

    // ��������� ����������� ��� ���������� ����
    Tree(Tree* up, SemNode* data = nullptr);

    // ��������������� ������� ������
    Tree* FindUp(const std::string& id);
    Tree* FindUpOneLevel(const std::string& id);
    Tree* DupControl(const std::string& id);

    void recursivePrint(const Tree* node, const std::string& prefix, bool isLast) const;
    std::string getDataTypeName(DATA_TYPE type) const;
    std::string getNodeInfo(const Tree* node) const;
};