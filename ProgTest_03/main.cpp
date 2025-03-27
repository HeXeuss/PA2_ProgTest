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
        _len = std::strlen(str);
        _data = new char[_len + 1];
        std::memcpy(_data, str, _len + 1);
    }
    String(const String& other){
        _len = other._len;
        _data = new char[_len + 1];
        std::memcpy(_data,other._data, _len + 1);
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
        std::memcpy(newData, _data, _len);
        std::memcpy(newData + _len, other._data, other._len + 1);
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
        return std::strcmp(_data, other._data) < 0;
    }
    bool operator>(const String & other) const {
        return std::strcmp(_data, other._data) > 0;
    }
    bool operator<=(const String & other) const {
        return std::strcmp(_data, other._data) <= 0;
    }
    bool operator>=(const String & other) const{
        return std::strcmp(_data, other._data) >= 0;
    }
    bool operator==(const String & other) const {
        return std::strcmp(_data, other._data) == 0;
    }
    bool operator!=(const String & other) const{
        return std::strcmp(_data, other._data) != 0;
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
    char * _data;
    size_t _len;
};

template <typename T>
class Array {
public:
    Array() : _size(0), _capacity(0), _data(nullptr) {}
    Array(const Array &other) : _size(other._size), _capacity(other._capacity), _data(new T[other._capacity]) {
        for (size_t i = 0; i < other._size; i++)
            _data[i] = other._data[i];
    }
    ~Array() {
        delete[] _data;
    }

    void swap(Array &other) noexcept {
        std::swap(_size, other._size);
        std::swap(_capacity, other._capacity);
        std::swap(_data, other._data);
    }

    Array &operator=(Array other) {
        swap(other);
        return *this;
    }

    T &operator[](size_t index) {
        return _data[index];
    }

    const T &operator[](size_t index) const {
        return _data[index];
    }

    void push_back(const T &value) {
        if (_size >= _capacity) {
            resize();
        }
        _data[_size++] = value;
    }

    void insert(size_t index, const T &value) {
        if (_size >= _capacity) {
            resize();
        }
        for (size_t i = _size; i > index; i--) {
            std::swap(_data[i], _data[i - 1]);
        }
        _data[index] = value;
        _size++;
    }

    void erase(const size_t index) {
        if (index >= _size) return;
        for (size_t i = index; i < _size - 1; i++) {
            _data[i] = std::move(_data[i + 1]);
        }
        _size--;
    }

    size_t size() const {
        return _size;
    }

    size_t end() const {
        return _size;
    }

    size_t find(const T &value) const {
        for (size_t i = 0; i < _size; i++) {
            if (_data[i] == value) return i;
        }
        return end();
    }

private:
    void resize() {
        _capacity = _capacity * 2 + 1;
        T *new_data = new T[_capacity];
        for (size_t i = 0; i < _size; i++) {
            new_data[i] = _data[i];
        }
        delete[] _data;
        _data = new_data;
    }
    size_t _size;
    size_t _capacity;
    T *_data;
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

    bool settle(const Address& address) {
        for (size_t i = 0; i < _addresses.size(); i++) {
            if (address < _addresses[i]) {
                _addresses.insert(i, address);
                return true;
            }
            if (address == _addresses[i]) {
                return false;
            }
        }
        _addresses.push_back(address);
        return true;
    }

    friend std::ostream& operator<<(std::ostream& os, const Person& person) {
        os << person._id << " " << person._name << " " << person._surname << std::endl;
        for (size_t i = 0; i < person._addresses.size(); ++i) {
            os << "  " << person._addresses[i] << std::endl;
        }
        os << "  ";
        return os;
    }

private:
    String _id;
    String _name;
    String _surname;
    Array<Address> _addresses;
};

class CRegister {
    Array<Person> _people;
public:
    CRegister() {
        _people = Array<Person>();
    }

    CRegister(const CRegister &other) {
        _people = other._people;
    }

    ~CRegister() = default;

    CRegister &operator=(CRegister other) {
        swap(other);
        return *this;
    }

    void swap(CRegister &other) noexcept {
        std::swap(_people, other._people);
    }

    bool add(const char id[], const char name[], const char surname[],
             const char date[], const char street[], const char city[]) {
        Person newPerson = Person(String(id), String(name),String(surname));
        size_t it = _people.find(newPerson);
        if (it != _people.end()) return false;
        newPerson.settle(Address(String(date), String(street), String(city)));
        _people.push_back(newPerson);

        return true;
    }

    bool resettle(const char id[], const char date[], const char street[], const char city[]) {
        size_t it_id = _people.find(Person(String(id), String(""), String("")));
        if (it_id == _people.end()) return false;
        Address newAddress = Address(String(date), String(street), String(city));
        return _people[it_id].settle(newAddress);
    }

    bool print(std::ostream &os, const char id[]) const {
        size_t it_id = _people.find(Person(String(id), String(""), String("")));
        if (it_id == _people.end()) return false;
        os << _people[it_id];
        return true;
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
