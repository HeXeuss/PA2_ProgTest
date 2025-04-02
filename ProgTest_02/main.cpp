#ifndef __PROGTEST__
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <climits>
#include <cmath>
#include <cfloat>
#include <cassert>
#include <unistd.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <span>
#include <algorithm>
#include <memory>
#include <compare>
#include <complex>
#endif /* __PROGTEST__ */

using namespace std;

class poly_var
{
public:
    poly_var(const string & newName ) {
        initialize(newName);
    }
    static void initialize(const string& newName) {
        varName = newName;
    }
    static string get() {
        return varName;
    }
    friend ostream& operator<<(ostream& out, const poly_var&) {
        return out;
    }
private:
    static string varName;
};
string poly_var::varName = "x";

class CPolynomial {
private:
    vector<double> _coefficients;
public:
    // Constructors and Destructor
    CPolynomial(): _coefficients(1,0.0){}
    CPolynomial(const CPolynomial& other):
            _coefficients(other._coefficients){}
    ~CPolynomial()= default;

    // Assignment operator
    CPolynomial& operator=(const CPolynomial& other){
        if (this == &other) {
            return *this;
        }
        _coefficients = other._coefficients;
        return *this;
    }

    // Stream insertion operator
    friend std::ostream& operator<<(std::ostream& os, const CPolynomial& p){
        bool first = true;

        for (int i = p._coefficients.size() - 1; i >= 0; i--) {
            double c = p._coefficients[i];
            if (c == 0) continue;

            if (!first) {
                if (c > 0) os << " + ";
                else os << " - ";
            } else if (c < 0) {
                os << "- ";
            }
            if (abs(c) != 1 || i == 0) {
                os << abs(c);
            }
            if (i > 0) {
                if (abs(c) != 1) os << "*";
                os << poly_var::get();
                os << "^" << i;
            }
            first = false;
        }
        if (first) os << "0";
        return os;
    }

    // Arithmetic operators
    CPolynomial& operator*=(double scalar){
        for (double & _coefficient : _coefficients) {
            _coefficient *= scalar;
        }
        return *this;
    }
    CPolynomial& operator*=(int scalar){
        *this *= static_cast<double>(scalar);
        return *this;
    }
    CPolynomial& operator*=(const CPolynomial& other){
        CPolynomial res;
        res._coefficients.resize(_coefficients.size() + other._coefficients.size() - 1, 0.0);
        for(size_t i = 0; i < _coefficients.size(); i++) {
            for(size_t j = 0; j < other._coefficients.size(); j++) {
                res[i + j] += _coefficients[i] * other._coefficients[j];
            }
        }
        *this = res;
        return *this;
    }
    CPolynomial operator*(double scalar)const{
        CPolynomial result(*this);
        result *= scalar;
        return result;
    }
    CPolynomial operator*(int scalar)const{
        CPolynomial result(*this);
        result *= static_cast<double>(scalar);
        return result;
    }
    CPolynomial operator*(const CPolynomial& other)const{
        CPolynomial result(*this);
        result *= other;
        return result;
    }

    friend CPolynomial operator*(double scalar, const CPolynomial & other){
        return other * scalar;
    }
    friend CPolynomial operator*(int scalar, const CPolynomial & other){
        return other * static_cast<double>(scalar);
    }


    // Comparison operators
    bool operator==(const CPolynomial& other) const{
        size_t size = max(_coefficients.size(), other._coefficients.size());

        for (size_t i = 0; i < size; ++i) {
            double c1 = 0.0;
            double c2 = 0.0;
            if (i < _coefficients.size()) {
                c1 = _coefficients[i];
            }

            if (i < other._coefficients.size()) {
                c2 = other._coefficients[i];
            }

            if (c1 != c2) {
                return false;
            }
        }

        return true;
    }
    bool operator!=(const CPolynomial& other) const{
        return !(*this == other);
    }

    // Subscript operator
    double& operator[](size_t index){
        if(index >= _coefficients.size()){
            _coefficients.resize(index + 1, 0.0);
        }
        return _coefficients[index];
    }
    double operator[](size_t index) const{
        if(index >= _coefficients.size()){
            return 0.0;
        }
        return _coefficients[index];
    }

    // Function call operator
    double operator()(double x) const{
        double res = 0;
        double power = 1;
        for(auto c : _coefficients){
            res += power * c;
            power *= x;
        }
        return res;
    }

    // Degree method
    size_t degree() const {
        size_t i = _coefficients.size();
        while(i--) {
            if (_coefficients[i] != 0.0) {
                return i;
            }
        }
        return 0;
    }

    // Type conversion operators
    explicit operator bool() const{
        for(double _coefficient : _coefficients){
            if(_coefficient != 0) return true;
        }
        return false;
    }
    bool operator!() const{
        return !static_cast<bool>(*this);
    }
};

#ifndef __PROGTEST__
bool smallDiff(double a, double b) {
    return std::abs(a - b) <= 0.001 * std::max(std::abs(a), std::abs(b));
}

bool dumpMatch ( const CPolynomial & x,
                 const std::vector<double> & ref )
{
    CPolynomial b;
    for(int i = 0 ; i<ref.size() ; i++){
        b[i] = ref[i];
    }
    return b == x;
}

int main ()
{
    CPolynomial a, b, c;
    std::ostringstream out, tmp;


    a[0] = -10;
    a[1] = 3.5;
    a[3] = 1;
    assert ( smallDiff ( a ( 2 ), 5 ) );
    out . str ("");
    out << a;
    assert ( out . str () == "x^3 + 3.5*x^1 - 10" );
    c = a * -2;
    assert ( c . degree () == 3
             && dumpMatch ( c, std::vector<double>{ 20.0, -7.0, -0.0, -2.0 } ) );

    out . str ("");
    out << c;
    assert ( out . str () == "- 2*x^3 - 7*x^1 + 20" );
    out . str ("");
    out << b;
    assert ( out . str () == "0" );
    b[5] = -1;
    b[2] = 3;
    out . str ("");
    out << b;
    assert ( out . str () == "- x^5 + 3*x^2" );
    c = a * b;
    assert ( c . degree () == 8
             && dumpMatch ( c, std::vector<double>{ -0.0, -0.0, -30.0, 10.5, -0.0, 13.0, -3.5, 0.0, -1.0 } ) );

    out . str ("");
    out << c;
    assert ( out . str () == "- x^8 - 3.5*x^6 + 13*x^5 + 10.5*x^3 - 30*x^2" );
    a *= 5;
    assert ( a . degree () == 3
             && dumpMatch ( a, std::vector<double>{ -50.0, 17.5, 0.0, 5.0 } ) );

    a *= b;
    assert ( a . degree () == 8
             && dumpMatch ( a, std::vector<double>{ 0.0, 0.0, -150.0, 52.5, -0.0, 65.0, -17.5, -0.0, -5.0 } ) );

    assert ( a != b );
    b[5] = 0;
    assert ( static_cast<bool> ( b ) );
    assert ( ! ! b );
    b[2] = 0;
    assert ( !(a == b) );
    a *= 0;
    assert ( a . degree () == 0
             && dumpMatch ( a, std::vector<double>{ 0.0 } ) );

    assert ( a == b );
    assert ( ! static_cast<bool> ( b ) );
    assert ( ! b );

    // bonus - manipulators

     out . str ("");
     out << poly_var ( "y" ) << c;
     assert ( out . str () == "- y^8 - 3.5*y^6 + 13*y^5 + 10.5*y^3 - 30*y^2" );
     out . str ("");
     tmp << poly_var ( "abc" );
     out . copyfmt ( tmp );
     out << c;
     assert ( out . str () == "- abc^8 - 3.5*abc^6 + 13*abc^5 + 10.5*abc^3 - 30*abc^2" );
    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
