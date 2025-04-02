#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#endif /* __PROGTEST__ */

class String {
public:

    explicit String(const char* str = ""){
        if (!str) str = "";
        _len =  strlen(str);
        _data = new char[_len + 1];
        memcpy(_data, str, _len + 1);
    }
    String(const String& other){
        _len = other._len;
        _data = new char[_len + 1];
        memcpy(_data,other._data, _len + 1);
    }
    ~String(){
        delete[] _data;
    }

    String& operator=(String other){
        swap(other);
        return *this;
    }

    String& operator+=(const String& other){
        size_t newLen = _len + other._len;
        char* newData = new char[newLen + 1];
        memcpy(newData, _data, _len);
        memcpy(newData + _len, other._data, other._len + 1);
        delete[] _data;
        _data = newData;
        _len = newLen;
        return *this;
    }
    friend String operator+(const String& a, const String & b){
        String newStr(a);
        newStr += b;
        return newStr;
    }

    bool operator<(const String & other) const {
        return strcmp(_data, other._data) < 0;
    }
    bool operator>(const String & other) const {
        return strcmp(_data, other._data) > 0;
    }
    bool operator<=(const String & other) const {
        return strcmp(_data, other._data) <= 0;
    }
    bool operator>=(const String & other) const{
        return strcmp(_data, other._data) >= 0;
    }
    bool operator==(const String & other) const {
        return strcmp(_data, other._data) == 0;
    }
    bool operator!=(const String & other) const{
        return strcmp(_data, other._data) != 0;
    }
    char operator[](const size_t index) const {
        return _data[index];
    }
    char &operator[](const size_t index){
        return _data[index];
    }

    friend std::ostream & operator<<(std::ostream & os, const String & str){
        os << str._data;
        return  os;
    }

    void swap(String &other) noexcept {
        std::swap(_data, other._data);
        std::swap(_len,  other._len);
    }

    size_t length() const {
        return _len;
    }
private:
    char* _data;
    size_t _len;
};

template <typename T>
class Set {
public:
    Set() : root(nullptr) {}
    ~Set() {
        destroy(root);
    }

    Set(const Set& other): root(nullptr){
        root = copyTree(other.root);
    }

    Set & operator=(Set other){
        std::swap(root, other.root);
        return *this;
    }

    void insert(const T& value) {
        root = insert(root, value);
    }

    T* find(const T& value) const {
        return find(root, value);
    }

    void print(std::ostream& os) const {
        print(root, os);
    }

private:
    struct Node {
        T value;
        Node* left;
        Node* right;
        int height;
        int balance;

        int getHeight(){
            updateNode();
            return height;
        }

        int getBalance(){
            updateNode();
            return balance;
        }

        void updateNode() {
            int leftHeight = 0;
            int rightHeight = 0;

            if (left != nullptr)  leftHeight  = left->height;
            if (right != nullptr) rightHeight = right->height;

            height = std::max(leftHeight, rightHeight) + 1;
            balance = leftHeight - rightHeight;
        }

        explicit Node(const T& value)
            : value(value), left(nullptr), right(nullptr), height(1), balance(0) {}

    };

    Node* root;

    Node * copyTree(Node * node){
        if(!node) return nullptr;
        Node* newNode = new Node(node->value);

        newNode->left = copyTree(node->left);
        newNode->right = copyTree(node->right);

        return newNode;

    }

    void print(Node* node, std::ostream& os) const {
        if (node) {
            print(node->left, os);
            os << "  " <<node->value << "\n";
            print(node->right, os);
        }
    }

    void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

    T* find( Node* node, const T& value) const {
        while (node) {
            if (value < node->value) {
                node = node->left;
            }
            else if (node->value < value) {
                node = node->right;
            }
            else return &(node->value);
        }
        return nullptr;
    }

    Node* rightRotation(Node* node) {
        Node* tmp = node->left;
        node->left = tmp->right;
        tmp->right = node;

        node->updateNode();
        tmp->updateNode();

        return tmp;
    }

    Node* leftRotation(Node* node) {
        Node* tmp = node->right;
        node->right = tmp->left;
        tmp->left = node;

        node->updateNode();
        tmp->updateNode();

        return tmp;
    }

    Node* balance(Node* node) {
        if (node->getBalance() == 2) {
            if (node->left->getBalance() < 0) {
                node->left = leftRotation(node->left);
            }
            return rightRotation(node);
        }
        if (node->getBalance() == -2) {
            if (node->right->getBalance() > 0) {
                node->right = rightRotation(node->right);
            }
            return leftRotation(node);
        }
        return node;
    }

    Node* insert(Node* node, const T& value) {
        if (!node) return new Node(value);

        if (value < node->value) {
             node->left = insert(node->left, value);
        }
        else if (node->value < value) {
             node->right = insert(node->right, value);
        }
        else return node;

        node->updateNode();
        return balance(node);
    }
};

class Address {
    public:
    explicit Address(const String& date = String(""), const String& street = String(""), const String& city = String(""))
    : _date(date), _street(street), _city(city) {}

    Address(const Address& other)
        : _date(other._date), _street(other._street), _city(other._city) {}

    Address& operator=(Address other) {
        swap(other);
        return *this;
    }

    void swap(Address &other) noexcept {
        std::swap(_date, other._date);
        std::swap(_street, other._street);
        std::swap(_city, other._city);
    }

    bool operator==(const Address& other) const {
        return _date == other._date;
    }

    bool operator<(const Address& other) const {
        return _date < other._date;
    }

    friend std::ostream& operator<<(std::ostream& os, const Address& address) {
        os << address._date << " " << address._street << " " << address._city;
        return os;
    }
private:
    String _date;
    String _street;
    String _city;
};

class Person {
public:
    explicit Person(const String& id = String(""), const String& name = String(""), const String& surname = String(""))
        : _id(id), _name(name), _surname(surname) {}

    Person(const Person& other)
        : _id(other._id), _name(other._name), _surname(other._surname), _addresses(other._addresses) {}

    Person& operator=(Person other) {
        swap(other);
        return *this;
    }

    void swap(Person &other) noexcept {
        std::swap(_id, other._id);
        std::swap(_name, other._name);
        std::swap(_surname, other._surname);
        std::swap(_addresses, other._addresses);
    }

    bool operator==(const Person& other) const {
        return _id == other._id;
    }

    bool operator!=(const Person& other) const {
        return !(*this == other);
    }

    bool operator<(const Person& other) const {
        return _id < other._id;
    }

    bool hasAddress(const Address& address) {
        if (_addresses.find(address) != nullptr) {
            return true;
        }
        return false;
    }

    bool settle(const Address& address) {
        if (hasAddress(address)) return false;
        _addresses.insert(address);
        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Person& person) {
        os << person._id << " " << person._name << " " << person._surname << "\n";
        person._addresses.print(os);
        os << "  ";
        return os;
    }

private:
    String _id;
    String _name;
    String _surname;
    Set<Address> _addresses;
};


class CRegister {
public:
    CRegister(): _data(new Set<Person>()), _refCount(new size_t(1)) {}

    CRegister(const CRegister &other) : _data(other._data), _refCount(other._refCount) {
        (*_refCount)++;
    }

    ~CRegister() {
        if (--(*_refCount) == 0) {
            delete _data;
            delete _refCount;
        }
    }

    CRegister &operator=(CRegister other) {
        swap(other);
        return *this;
    }
    void swap(CRegister &other) noexcept {
        std::swap(_data, other._data);
        std::swap(_refCount, other._refCount);
    }

    bool add(const char id[], const char name[], const char surname[], const char date[], const char street[], const char city[]) {

        Person newPerson = Person(String(id), String(name),String(surname));

        if (_data->find(newPerson) != nullptr) return false;
        detach();
        newPerson.settle(Address(String(date), String(street), String(city)));
        _data->insert(newPerson);
        return true;
    }

    bool resettle(const char id[], const char date[], const char street[], const char city[]) {
        Person * it_id = _data->find(Person(String(id), String(""), String("")));
        if (it_id == nullptr) return false;

        Address newAddress = Address(String(date), String(street), String(city));
        if (it_id->hasAddress(newAddress)) return false;

        detach();

        it_id = _data->find(Person(String(id), String(""), String("")));
        if (it_id == nullptr) return false;

        return it_id->settle(newAddress);
    }

    bool print(std::ostream &os, const char id[]) const {
        Person * it_id = _data->find(Person(String(id), String(""), String("")));
        if (it_id == nullptr) return false;

        os << *it_id;
        return true;
    }
private:
    Set<Person> * _data;
    size_t * _refCount;

    void detach() {
        if (*_refCount == 1) return;
        Set<Person> *newData = new Set<Person>(*_data);
        size_t *newRefCount = new size_t(1);
        if (--(*_refCount) == 0) {
            delete _data;
            delete _refCount;
        }
        _data = newData;
        _refCount = newRefCount;
    }

};

#ifndef __PROGTEST__
int main ()
{
    char   lID[12], lDate[12], lName[50], lSurname[50], lStreet[50], lCity[50];
    std::ostringstream oss;
    CRegister  a;
    assert ( a . add ( "123456/7890", "John", "Smith", "2000-01-01", "Main street", "Seattle" ) == true );
    assert ( a . add ( "987654/3210", "Freddy", "Kruger", "2001-02-03", "Elm street", "Sacramento" ) == true );
    assert ( a . resettle ( "123456/7890", "2003-05-12", "Elm street", "Atlanta" ) == true );
    assert ( a . resettle ( "123456/7890", "2002-12-05", "Sunset boulevard", "Los Angeles" ) == true );
    oss . str ( "" );
    assert ( a . print ( oss, "123456/7890" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(123456/7890 John Smith
  2000-01-01 Main street Seattle
  2002-12-05 Sunset boulevard Los Angeles
  2003-05-12 Elm street Atlanta
  )###" ) );
    oss . str ( "" );
    assert ( a . print ( oss, "987654/3210" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
  2001-02-03 Elm street Sacramento
  )###" ) );
    CRegister b ( a );
    assert ( b . resettle ( "987654/3210", "2008-04-12", "Elm street", "Cinccinati" ) == true );
    assert ( a . resettle ( "987654/3210", "2007-02-11", "Elm street", "Indianapolis" ) == true );
    oss . str ( "" );
    assert ( a . print ( oss, "987654/3210" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
  2001-02-03 Elm street Sacramento
  2007-02-11 Elm street Indianapolis
  )###" ) );
    oss . str ( "" );
    assert ( b . print ( oss, "987654/3210" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
  2001-02-03 Elm street Sacramento
  2008-04-12 Elm street Cinccinati
  )###" ) );
  a = b;
  assert ( a . resettle ( "987654/3210", "2011-05-05", "Elm street", "Salt Lake City" ) == true );
    oss . str ( "" );
    assert ( a . print ( oss, "987654/3210" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
  2001-02-03 Elm street Sacramento
  2008-04-12 Elm street Cinccinati
  2011-05-05 Elm street Salt Lake City
  )###" ) );
    oss . str ( "" );
    assert ( b . print ( oss, "987654/3210" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(987654/3210 Freddy Kruger
  2001-02-03 Elm street Sacramento
  2008-04-12 Elm street Cinccinati
  )###" ) );
    assert ( b . add ( "987654/3210", "Joe", "Lee", "2010-03-17", "Abbey road", "London" ) == false );
    assert ( a . resettle ( "987654/3210", "2001-02-03", "Second street", "Milwaukee" ) == false );
    oss . str ( "" );
    assert ( a . print ( oss, "666666/6666" ) == false );

    CRegister  c;
    strncpy ( lID, "123456/7890", sizeof ( lID ) );
    strncpy ( lName, "John", sizeof ( lName ) );
    strncpy ( lSurname, "Smith", sizeof ( lSurname ) );
    strncpy ( lDate, "2000-01-01", sizeof ( lDate) );
    strncpy ( lStreet, "Main street", sizeof ( lStreet ) );
    strncpy ( lCity, "Seattle", sizeof ( lCity ) );
    assert (  c . add ( lID, lName, lSurname, lDate, lStreet, lCity ) == true );
    strncpy ( lID, "987654/3210", sizeof ( lID ) );
    strncpy ( lName, "Freddy", sizeof ( lName ) );
    strncpy ( lSurname, "Kruger", sizeof ( lSurname ) );
    strncpy ( lDate, "2001-02-03", sizeof ( lDate) );
    strncpy ( lStreet, "Elm street", sizeof ( lStreet ) );
    strncpy ( lCity, "Sacramento", sizeof ( lCity ) );
    assert (  c . add ( lID, lName, lSurname, lDate, lStreet, lCity ) == true );
    strncpy ( lID, "123456/7890", sizeof ( lID ) );
    strncpy ( lDate, "2003-05-12", sizeof ( lDate) );
    strncpy ( lStreet, "Elm street", sizeof ( lStreet ) );
    strncpy ( lCity, "Atlanta", sizeof ( lCity ) );
    assert ( c . resettle ( lID, lDate, lStreet, lCity ) == true );
    strncpy ( lID, "123456/7890", sizeof ( lID ) );
    strncpy ( lDate, "2002-12-05", sizeof ( lDate) );
    strncpy ( lStreet, "Sunset boulevard", sizeof ( lStreet ) );
    strncpy ( lCity, "Los Angeles", sizeof ( lCity ) );
    assert ( c . resettle ( lID, lDate, lStreet, lCity ) == true );
    oss . str ( "" );
    assert ( c . print ( oss, "123456/7890" ) == true );
    assert ( ! strcmp ( oss . str () . c_str (), R"###(123456/7890 John Smith
  2000-01-01 Main street Seattle
  2002-12-05 Sunset boulevard Los Angeles
  2003-05-12 Elm street Atlanta
  )###" ) );

  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
