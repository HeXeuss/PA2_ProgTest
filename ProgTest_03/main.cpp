#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#endif /* __PROGTEST__ */

class CRegister {
public:
    // Default constructor: Initializes an empty registry.
    CRegister();

    // Copy constructor: Creates a deep copy of another instance.
    CRegister(const CRegister &other);

    // Destructor: Cleans up allocated resources.
    ~CRegister();

    // Assignment operator: Performs deep copy to avoid memory leaks.
    CRegister &operator=(const CRegister &other);

    /**
     * Adds a new person to the registry.
     * @param id Unique identifier for the person (format "XXXXXX/YYYY").
     * @param name First name of the person (arbitrary length).
     * @param surname Last name of the person (arbitrary length).
     * @param date Date of residence validity start (format "YYYY-MM-DD").
     * @param street Street address (arbitrary length and content).
     * @param city City of residence (arbitrary length and content).
     * @return True if successfully added, False if an entry with the same ID already exists.
     */
    bool add(const char id[], const char name[], const char surname[],
             const char date[], const char street[], const char city[]);

    /**
     * Updates an existing person's address in the registry.
     * @param id Unique identifier of the person.
     * @param date Date of residence change (format "YYYY-MM-DD").
     * @param street New street address.
     * @param city New city.
     * @return True if successfully updated, False if the ID does not exist
     *         or if an address change already exists for that date.
     */
    bool resettle(const char id[], const char date[], const char street[], const char city[]);

    /**
     * Prints the record of a person with the given ID.
     * @param os Output stream to print to.
     * @param id Unique identifier of the person.
     * @return True if the person exists and was printed, False otherwise.
     * @note Addresses are printed in ascending order of the change date.
     */
    bool print(std::ostream &os, const char id[]) const;

private:
    // Internal data structure to store records.
    // TODO: Define appropriate storage for the registry.
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
