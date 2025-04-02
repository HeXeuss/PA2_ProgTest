#ifndef __PROGTEST__
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cassert>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>
#include <memory>
#include <compare>
#include <functional>
#include <stdexcept>
#include <algorithm>
#endif /* __PROGTEST__ */

using namespace std;

struct Citizen{
    string name;
    string address;
    string account;
    int income = 0;
    int expenses = 0;
    Citizen(const string & name,const string & address, const string & account):
            name(name), address(address), account(account){};
    bool operator<(const Citizen & other){
        return tie(name,address) <tie(other.name,other.address);
    }
};

class CIterator
{
public:
    CIterator(const vector<shared_ptr<Citizen>> & data) : data(data){}
    bool atEnd () const{
        if(index >= data.size()) return true;
        return false;
    }
    void next (){
        if(index < data.size()) index++;
    }
    const std::string & name () const {
        static const string emptyStr;
        if(atEnd()) return emptyStr;
        return data[index]->name;
    }

    const std::string & addr () const {
        static const string emptyStr;
        if(atEnd()) return emptyStr;
        return data[index]->address;
    }

    const std::string & account () const {
        static const string emptyStr;
        if(atEnd()) return emptyStr;
        return data[index]->account;
    }
private:
    const vector<shared_ptr<Citizen>> & data;
    size_t index = 0;
};

bool nameCmp(const shared_ptr<Citizen>& a, const shared_ptr<Citizen> & b){
    return tie(a->name, a->address) < tie(b->name, b->address);
}
bool accountCmp(const shared_ptr<Citizen>& a, const shared_ptr<Citizen> & b){
    return a->account < b->account;
}

class CTaxRegister
{
public:
    bool birth (const std::string & name, const std::string & addr, const std::string & account){
        auto newCitizen = make_shared<Citizen>(name,addr,account);

        auto names_it = lower_bound(dataByNames.begin(), dataByNames.end(), newCitizen, nameCmp);
        if(names_it != dataByNames.end() && (*names_it)->name == name && (*names_it)->address == addr) return false;

        auto account_it = lower_bound(dataByAccounts.begin(), dataByAccounts.end(), newCitizen, accountCmp);
        if(account_it != dataByAccounts.end() && (*account_it)->account == account) return false;

        dataByNames.emplace(names_it, newCitizen);
        dataByAccounts.emplace(account_it,newCitizen);
        return true;
    }


    bool death (const std::string & name, const std::string & addr){
        auto deadCitizen = make_shared<Citizen>(name,addr, "");

        auto names_it = lower_bound(dataByNames.begin(), dataByNames.end(), deadCitizen, nameCmp);
        if(names_it == dataByNames.end() || (*names_it)->name != name || (*names_it)->address != addr) return false;

        auto account_it = lower_bound(dataByAccounts.begin(), dataByAccounts.end(), (*names_it), accountCmp);
        if(account_it == dataByAccounts.end() || (*account_it)->account != (*names_it)->account) return false;

        dataByNames.erase(names_it);
        dataByAccounts.erase(account_it);
        return true;
    }

    bool income (const std::string & account, int amount){
        auto citizen = make_shared<Citizen>("","",account);

        auto it = lower_bound(dataByAccounts.begin(),dataByAccounts.end(), citizen, accountCmp);
        if(it == dataByAccounts.end() || (*it)->account != account) return false;

        (*it)->income += amount;
        return true;
    }
    bool income (const std::string & name, const std::string & addr, int amount){
        auto citizen = make_shared<Citizen>(name, addr, "");

        auto it = lower_bound(dataByNames.begin(),dataByNames.end(), citizen, nameCmp);
        if(it == dataByNames.end() || (*it)->name != name || (*it)->address != addr) return false;

        (*it)->income += amount;
        return true;
    }
    bool expense (const std::string & account, int amount){
        auto citizen = make_shared<Citizen>("","",account);

        auto it = lower_bound(dataByAccounts.begin(),dataByAccounts.end(), citizen, accountCmp);
        if(it == dataByAccounts.end() || (*it)->account != account) return false;

        (*it)->expenses += amount;
        return true;
    }
    bool expense (const std::string & name, const std::string& addr, int amount){
        auto citizen = make_shared<Citizen>(name, addr, "");

        auto it = lower_bound(dataByNames.begin(),dataByNames.end(), citizen, nameCmp);
        if(it == dataByNames.end() || (*it)->name != name || (*it)->address != addr) return false;

        (*it)->expenses += amount;
        return true;
    }
    bool audit (const std::string & name, const std::string & addr,
                std::string & account, int & sumIncome, int & sumExpense) const {
        auto citizen = make_shared<Citizen>(name,addr,"");

        auto it = lower_bound(dataByNames.begin(), dataByNames.end(), citizen, nameCmp);
        if(it == dataByNames.end() || (*it)->name != name ||(*it)->address != addr) return false;

        account = (*it)->account;
        sumIncome = (*it)->income;
        sumExpense = (*it)->expenses;
        return true;
    }
    CIterator listByName () const{
        return CIterator(dataByNames);
    }
private:
    vector<shared_ptr<Citizen>> dataByNames;
    vector<shared_ptr<Citizen>> dataByAccounts;
};

#ifndef __PROGTEST__
int main ()
{
    int a[] = {1,2,3,4,5};


    std::string acct;
    int    sumIncome, sumExpense;
    CTaxRegister b0;
    assert ( b0 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
    assert ( b0 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
    assert ( b0 . birth ( "Peter Hacker", "Main Street 17", "634oddT" ) );
    assert ( b0 . birth ( "John Smith", "Main Street 17", "Z343rwZ" ) );
    assert ( b0 . income ( "Xuj5#94", 1000 ) );
    assert ( b0 . income ( "634oddT", 2000 ) );
    assert ( b0 . income ( "123/456/789", 3000 ) );
    assert ( b0 . income ( "634oddT", 4000 ) );
    assert ( b0 . income ( "Peter Hacker", "Main Street 17", 2000 ) );
    assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 2000 ) );
    assert ( b0 . expense ( "John Smith", "Main Street 17", 500 ) );
    assert ( b0 . expense ( "Jane Hacker", "Main Street 17", 1000 ) );
    assert ( b0 . expense ( "Xuj5#94", 1300 ) );
    assert ( b0 . audit ( "John Smith", "Oak Road 23", acct, sumIncome, sumExpense ) );
    assert ( acct == "123/456/789" );
    assert ( sumIncome == 3000 );
    assert ( sumExpense == 0 );
    assert ( b0 . audit ( "Jane Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
    assert ( acct == "Xuj5#94" );
    assert ( sumIncome == 1000 );
    assert ( sumExpense == 4300 );
    assert ( b0 . audit ( "Peter Hacker", "Main Street 17", acct, sumIncome, sumExpense ) );
    assert ( acct == "634oddT" );
    assert ( sumIncome == 8000 );
    assert ( sumExpense == 0 );
    assert ( b0 . audit ( "John Smith", "Main Street 17", acct, sumIncome, sumExpense ) );
    assert ( acct == "Z343rwZ" );
    assert ( sumIncome == 0 );
    assert ( sumExpense == 500 );
    CIterator it = b0 . listByName ();
    assert ( ! it . atEnd ()
             && it . name () == "Jane Hacker"
             && it . addr () == "Main Street 17"
             && it . account () == "Xuj5#94" );
    it . next ();
    assert ( ! it . atEnd ()
             && it . name () == "John Smith"
             && it . addr () == "Main Street 17"
             && it . account () == "Z343rwZ" );
    it . next ();
    assert ( ! it . atEnd ()
             && it . name () == "John Smith"
             && it . addr () == "Oak Road 23"
             && it . account () == "123/456/789" );
    it . next ();
    assert ( ! it . atEnd ()
             && it . name () == "Peter Hacker"
             && it . addr () == "Main Street 17"
             && it . account () == "634oddT" );
    it . next ();
    assert ( it . atEnd () );

    assert ( b0 . death ( "John Smith", "Main Street 17" ) );

    CTaxRegister b1;
    assert ( b1 . birth ( "John Smith", "Oak Road 23", "123/456/789" ) );
    assert ( b1 . birth ( "Jane Hacker", "Main Street 17", "Xuj5#94" ) );
    assert ( !b1 . income ( "634oddT", 4000 ) );
    assert ( !b1 . expense ( "John Smith", "Main Street 18", 500 ) );
    assert ( !b1 . audit ( "John Nowak", "Second Street 23", acct, sumIncome, sumExpense ) );
    assert ( !b1 . death ( "Peter Nowak", "5-th Avenue" ) );
    assert ( !b1 . birth ( "Jane Hacker", "Main Street 17", "4et689A" ) );
    assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
    assert ( b1 . death ( "Jane Hacker", "Main Street 17" ) );
    assert ( b1 . birth ( "Joe Hacker", "Elm Street 23", "Xuj5#94" ) );
    assert ( b1 . audit ( "Joe Hacker", "Elm Street 23", acct, sumIncome, sumExpense ) );
    assert ( acct == "Xuj5#94" );
    assert ( sumIncome == 0 );
    assert ( sumExpense == 0 );
    assert ( !b1 . birth ( "Joe Hacker", "Elm Street 23", "AAj5#94" ) );



    return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
