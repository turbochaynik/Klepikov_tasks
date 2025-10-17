#include "string_list.h"
#include <algorithm>
#include <iostream>
std::string StringList::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r\f\v");
    if (start == std::string::npos) return "";

    size_t end = str.find_last_not_of(" \t\n\r\f\v");
    return str.substr(start, end - start + 1);
}


StringList::Node::Node(const char* str) {
    data = new char[strlen(str) + 1];
    strcpy(data, str);
    next = nullptr;
}

StringList::Node::~Node() {
    delete[] data;
}

void StringList::copyFrom(const StringList& other) {
    clear();
    Node* current = other.head;
    Node** lastPtr = &head;

    while (current) {
        *lastPtr = new Node(current->data);
        lastPtr = &((*lastPtr)->next);
        current = current->next;
    }
    size = other.size;
}

void StringList::clear() {
    while (head) {
        Node* temp = head;
        head = head->next;
        delete temp;
    }
    size = 0;
}

StringList::StringList() : head(nullptr), size(0) {}

StringList::StringList(const char* str) : head(nullptr), size(0) {
    if (!str) throw Error("Передана пустая строка", __LINE__);
    head = new Node(str);
    size = 1;
}

StringList::StringList(const StringList& other) : head(nullptr), size(0) {
    copyFrom(other);
}

StringList::~StringList() {
    clear();
}

StringList& StringList::operator=(const StringList& other) {
    if (this != &other) {
        copyFrom(other);
    }
    return *this;
}

StringList StringList::operator+(const StringList& other) const {
    StringList result(*this);
    result += other;
    return result;
}

StringList& StringList::operator+=(const StringList& other) {
    if (!other.head) return *this;

    Node** lastPtr = &head;
    while (*lastPtr) {
        lastPtr = &((*lastPtr)->next);
    }

    Node* current = other.head;
    while (current) {
        std::string trimmedData = trim(current->data); // Обрезаем пробелы перед добавлением
        *lastPtr = new Node(trimmedData.c_str());
        lastPtr = &((*lastPtr)->next);
        current = current->next;
    }
    size += other.size;

    return *this;
}


StringList StringList::operator+(const char* str) const {
    if (!str) throw Error("Передана пустая строка", __LINE__);

    StringList result(*this);
    Node** lastPtr = &result.head;
    while (*lastPtr) {
        lastPtr = &((*lastPtr)->next);
    }

    std::string trimmedStr = trim(str);
    *lastPtr = new Node(trimmedStr.c_str());
    result.size++;
    return result;
}

StringList operator+(const char* str, const StringList& list) {
    if (!str) throw StringList::Error("Передана пустая строка", __LINE__);
    StringList result(str);
    result += list;
    return result;
}

StringList StringList::operator-(const char* str) const {
    if (!str) throw Error("Передана пустая строка", __LINE__);

    StringList result;
    Node* current = head;
    std::string trimmedStr = trim(str);

    while (current) {
        if (trim(current->data) != trimmedStr) {
            result += current->data;
        }
        current = current->next;
    }
    return result;
}

char* StringList::operator[](int index) const {
    if (index < 0 || index >= size) throw Error("Индекс вне границ", __LINE__);

    Node* current = head;
    for (int i = 0; i < index; ++i) {
        current = current->next;
    }

    char* copy = new char[strlen(current->data) + 1];
    strcpy(copy, current->data);
    return copy;
}

StringList::operator char*() const {
    int total_length = 0;
    Node* current = head;

    while (current) {
        total_length += strlen(current->data) + 1;
        current = current->next;
    }

    char* result = new char[total_length];
    result[0] = '\0';

    current = head;
    while (current) {
        strcat(result, current->data);
        strcat(result, "\n");
        current = current->next;
    }

    return result;
}

int StringList::length() const {
    return size;
}

void StringList::print() const {
    Node* current = head;
    while (current) {
        std::cout << current->data << std::endl;
        current = current->next;
    }
}


StringList StringList::pairLists(const StringList& other) const {
    if (this->size != other.size) {
        throw Error("Списки имеют разную длину", __LINE__);
    }

    StringList result;
    Node* currentThis = this->head;
    Node* currentOther = other.head;

    while (currentThis && currentOther) {
        std::string pairedString = std::string(currentThis->data) + "\n" + std::string(currentOther->data);
        result += pairedString.c_str();

        currentThis = currentThis->next;
        currentOther = currentOther->next;
    }

    return result;
}


bool StringList::contains(const char* str, bool space) const {
    if (!str) throw Error("Передана пустая строка", __LINE__);

    Node* current = head;
    std::string searchStr = str;

    if (!space) {
        searchStr = trim(searchStr);
    }

    while (current) {
        std::string nodeData = current->data;

        if (!space) {
            nodeData = trim(nodeData);
        }

        if (nodeData == searchStr) {
            return true;
        }

        current = current->next;
    }

    return false;
}

std::ostream& operator<<(std::ostream& os, const StringList& list) {
    StringList::Node* current = list.head;
    while (current) {
        os << current->data << '\n';
        current = current->next;
    }
    return os;  
}
