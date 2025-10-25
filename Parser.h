#pragma once
#include "TScaner.h"
#include "SemTree.h"
#include <string>
#include <iostream>
#include <stdexcept>
#include <cstring>
#include <vector>

class Parser {
private:
    TScaner scaner;
    int curTok;
    TypeLex curLex;
    Tree semTree;

    // ���������������
    void advance();                       // �������� ��������� �����
    void match(int expected);             // ������� ���������� �����
    bool accept(int t);                   // ���� ������� == t -> advance � return true
    int  peekNextToken(TypeLex outLex);   // ��������� ������: ������� ��� ���������� ������, �� ����� ���������

    void error(const std::string& msg);
    void semantic_error(const std::string& msg); // ��� ������������� ������

    // ����������� (�� ����������)
    void parseProgram();   // <���������> -> { <��������> }*
    void parseDescription(); // <��������> -> <������> | <�������> (D_s)
    void parseData();      // D -> T L ;
    DATA_TYPE parseType();      // <���> - ����� ���������� ������������� ���
    void parseL(DATA_TYPE type);         // L -> V { , V }
    void parseV(DATA_TYPE type);         // V -> i <������ ��������> | B
    void parseB();         // B -> i = E
    void parseIndices(std::vector<int>& dims); //��������� ������ ��������
    void parseIndices();   // <������ ��������> -> { '[' E ']' }*
    void parseH();         // H -> sequence of D or O (body)
    void parseO();         // O -> ; | { H } | F | B;
    void parseF();         // switch-case
    void parseCaseBranch(); // case branch

    // ������������� / ���������
    void expectIdentifier();
    void expectConstant();

    // ��������� (��� ����� ��������)
    DATA_TYPE parseE(); // E -> C { || C }
    DATA_TYPE parseC(); // C -> R { && R }
    DATA_TYPE parseR(); // R -> Q { (== | !=) Q }
    DATA_TYPE parseQ(); // Q -> A { (< | > | <= | >=) A }
    DATA_TYPE parseA(); // A -> M { (+|-) M }
    DATA_TYPE parseM(); // M -> U { (*|/|%) U }
    DATA_TYPE parseU(); // U -> !U | +U | -U | P
    DATA_TYPE parseP(); // P -> i | c | (E) | i <indices>

    // --- ��������������� ������������� ������� ---
    bool isIntegerType(DATA_TYPE t) const;
    bool isNumericType(DATA_TYPE t) const;
    void checkAssignmentCompatibility(DATA_TYPE left, DATA_TYPE right);
    void checkLogicalCompatibility(DATA_TYPE type);
    void checkRelationalCompatibility(DATA_TYPE left, DATA_TYPE right);
    void checkEqualityCompatibility(DATA_TYPE left, DATA_TYPE right);
    void parseDeclarationIndices(std::vector<int>& dims);
    int parseUsageIndices();
public:
    Parser(const char* filename);
    void parseAll(); // ��������� ������ (� ���������� ����)
};