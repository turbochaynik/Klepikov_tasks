#ifndef STRING_LIST_H
#define STRING_LIST_H

#include <iostream>
#include <cstring>

class StringList {
private:
    struct Node {
        char* data;
        Node* next;

        Node(const char* str);
        ~Node();
    };

    Node* head;
    int size;

    void copyFrom(const StringList& other);
    void clear();


friend std::ostream& operator<<(std::ostream& os, const StringList& list);

    static std::string trim(const std::string& str);/*
    bool contains(const char* str, bool space = false) const;
*/
public:
    bool contains(const char* str, bool space = false) const;
    class Error {
    private:
        std::string message;
    public:
        Error(const std::string& msg, int line) {
            message = "Ошибка (строка " + std::to_string(line) + "): " + msg;
        }
        const std::string& what() const { return message; }
    };

    StringList();
    StringList(const char* str);
    StringList(const StringList& other);
    ~StringList();

    StringList& operator=(const StringList& other);

    StringList operator+(const StringList& other) const;
    StringList& operator+=(const StringList& other);
    StringList operator+(const char* str) const;
    friend StringList operator+(const char* str, const StringList& list);
    StringList operator-(const char* str) const;
    StringList pairLists(const StringList& other) const;

    char* operator[](int index) const;
    operator char*() const;
    int length() const;

    void print() const;



};

#endif // STRING_LIST_H

