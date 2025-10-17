Я делал вариант 1.2.
Программа у меня состоит из string_list.h, string_list.cpp и main.cpp, реализующего методы.
string_list.h:
1. Защищенная структура Node (внутренний узел списка). Каждый узел списка хранит:
data — строка (char*), динамически выделяемая.
next — указатель на следующий узел в списке.
Имеются конструктор (создает узел) и деструктор (освобождает память).

2. Приватные члены класса:
head указывает на первый элемент списка.
size хранит количество элементов.

Приватные методы:
copyFrom() — копирует содержимое другого объекта StringList при создании копии.
clear() — очищает список, освобождая память.

3. Конструкторы и деструктор
Конструктор по умолчанию (StringList()) — создает пустой список.
Конструктор StringList(const char* str) — создает список из одной строки.
Конструктор копирования (StringList(const StringList& other)) — копирует существующий список.
Деструктор (~StringList()) — освобождает память.

4. Операторы присваивания и арифметики
Перегруженный оператор = выполняет глубокое копирование списка.
operator+ объединяет два списка, создавая новый.
operator+= добавляет другой список к текущему.
StringList operator+(const char* str) const;
friend StringList operator+(const char* str, const StringList& list);
Добавляют строку к списку.
StringList operator-(const char* str) const;
Удаляет указанную строку из списка (если она есть).
StringList pairLists(const StringList& other) const;
Метод для парного объединения списков. 

5. Операторы доступа
operator[] позволяет обращаться к элементу списка по индексу.
operator char*() преобразует объект StringList в char*.

6. Другие методы
int length() const;
Возвращает длину списка.
void print() const;
Выводит список в консоль.
bool contains(const char* str, bool space = false) const;
Проверяет, содержится ли строка в списке.
static std::string trim(const std::string& str);
Метод для удаления пробелов (но почему-то закомментирован).

7. Обработка ошибок
Класс Error используется для представления ошибок.
Хранит сообщение об ошибке с указанием строки.

8. Оператор вывода
friend std::ostream& operator<<(std::ostream& os, const StringList& list);
Позволяет выводить StringList в std::cout как обычный текст.

string_list.cpp:

1. Метод trim()
Назначение: удаляет пробелы в начале и конце строки.
Используется в: operator+=, operator-, contains, operator+.

2. Вложенный класс Node
Конструктор Node:
Выделяет память под строку (data) и копирует str.
next = nullptr (по умолчанию узел никуда не указывает).
Деструктор ~Node:
Освобождает память, выделенную под data.

3. Приватные методы
void StringList::copyFrom(const StringList& other) {
Назначение: копирует содержимое other в текущий объект.
void StringList::clear() {
Назначение: удаляет все узлы списка, освобождая память.

4. Конструкторы и деструктор
StringList::StringList() : head(nullptr), size(0) {}
Создает пустой список.
StringList::StringList(const char* str) : head(nullptr), size(0) {
Создает список из одной строки.
StringList::StringList(const StringList& other) : head(nullptr), size(0) {
Создает копию существующего списка.
StringList::~StringList() {
Освобождает память при удалении объекта.

5. Оператор присваивания =
StringList& StringList::operator=(const StringList& other) {
Предотвращает самоприсваивание (this != &other).
Очищает текущий список и копирует other.

6. Операции сложения + и +=
StringList StringList::operator+(const StringList& other) const {
Создает новый список result и добавляет в него other.
StringList& StringList::operator+=(const StringList& other) {
Удаляет пробелы перед добавлением.
StringList StringList::operator+(const char* str) const {
Добавляет строку в конец списка с удалением пробелов.

7. Оператор - (удаление элемента)
StringList StringList::operator-(const char* str) const {
Удаляет строку без учета пробелов.

8. Доступ по индексу []
char* StringList::operator[](int index) const {
Возвращает копию строки (не саму строку, чтобы избежать изменений оригинала).

9. Преобразование в char*
StringList::operator char*() const {
Объединяет все строки в одну с разделением \n.

10. Оператор вывода <<
std::ostream& operator<<(std::ostream& os, const StringList& list) {
Выводит список в std::cout.
