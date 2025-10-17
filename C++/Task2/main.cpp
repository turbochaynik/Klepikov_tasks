#include "string_list.h"

int main() {
    // Тест 1: Создание списка из строки
    StringList list1(" abc ");
    StringList list2(" def ");

    std::cout << "Список 1 (начальный):\n" << list1;
    std::cout << "Список 2 (начальный):\n" << list2;

    // Тест 2: Конкатенация списков (list + list)
    StringList list3 = list1 + list2;
    std::cout << "Список 3 (конкатенация списка 1 и списка 2):\n" << list3;

    // Тест 3: Оператор += (list1 += list2)
    list1 += list2;
    std::cout << "Список 1 после list1 += list2:\n" << list1;

    // Тест 4: Добавление строки в начало списка (char* + list)
    StringList list4 = " xyz " + list2;
    std::cout << "Список 4 (char* + список 2):\n" << list4;

    // Тест 5: Добавление строки в конец списка (list + char*)
    StringList list5 = list1 + " 123 ";
    std::cout << "Список 5 (список 1 + '123'):\n" << list5;

    // Тест 6: Удаление элемента из списка (list - char*)
    StringList list6 = list1 - "def";
    std::cout << "Список 6 (список 1 - ' def '):\n" << list6;

    // Тест 7: Доступ к элементу по индексу (list[i])
    char* copy_str = list3[1];
    std::cout << "Элемент с индексом 1 списка 3: " << copy_str << std::endl;
    delete[] copy_str;

    // Тест 8: Приведение списка к строке (оператор char*)
    char* str_repr = list1;
    std::cout << "Строковое представление списка 1:\n" << str_repr;
    delete[] str_repr;

    // Тест 9: Получение длины списка
    std::cout << "Длина списка 1: " << list1.length() << std::endl;


    StringList list7("one");
    list7 += "two";
    list7 += "three";

    StringList list8("uno");
    list8 += "dos";
    list8 += "tres";

    std::cout << "Список 7 (начальный):\n" << list7;
    std::cout << "Список 8 (начальный):\n" << list8;

    StringList pairedList = list7.pairLists(list8);
    std::cout << "Список с парами из списка 7 и списка 8:\n" << pairedList;


    // Тест 10: Проверка вхождения строки в список с учетом пробелов
    bool found = list1.contains("abc ", true);
    std::cout << "Содержит ли список 1 строку 'abc ' (с пробелами)? " << (found ? "Да" : "Нет") << std::endl;

    // Тест 11: Проверка вхождения строки в список без учета пробелов
    found = list1.contains("abc", false);
    std::cout << "Содержит ли список 1 строку 'abc' (без пробелов)? " << (found ? "Да" : "Нет") << std::endl;

    // Тест 12: Проверка вхождения строки, которой нет в списке
    found = list1.contains("xyz", false);
    std::cout << "Содержит ли список 1 строку 'xyz'? " << (found ? "Да" : "Нет") << std::endl;

   return 0;

}

