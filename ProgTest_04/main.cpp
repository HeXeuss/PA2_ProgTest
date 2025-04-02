#ifndef __PROGTEST__
#include <cassert>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <algorithm>
#include <memory>
#include <compare>
#include <functional>
#include <optional>

class CTimeStamp {
public:
    CTimeStamp(int year, int month, int day, int hour, int minute, double sec)
        : _year(year), _month(month), _day(day), _hour(hour), _minute(minute), _sec(sec) {}

    int compare(const CTimeStamp &x) const {
        if (_year != x._year) return _year < x._year ? -1 : 1;
        if (_month != x._month) return _month < x._month ? -1 : 1;
        if (_day != x._day) return _day < x._day ? -1 : 1;
        if (_hour != x._hour) return _hour < x._hour ? -1 : 1;
        if (_minute != x._minute) return _minute < x._minute ? -1 : 1;
        if (_sec != x._sec) return _sec < x._sec ? -1 : 1;
        return 0;
    }

    friend std::ostream &operator<<(std::ostream &os, const CTimeStamp &x) {
        os << std::setfill('0') << std::setw(4) << x._year << "-"
           << std::setw(2) << x._month << "-"
           << std::setw(2) << x._day << " "
           << std::setw(2) << x._hour << ":"
           << std::setw(2) << x._minute << ":"
           << std::fixed << std::setprecision(3) << x._sec;
        return os;
    }

private:
    int _year, _month, _day, _hour, _minute;
    double _sec;
};

class CMail {
public:
    CMail(const CTimeStamp &timeStamp, const std::string &from, const std::string &to, const std::optional<std::string> &subject)
        : _timeStamp(timeStamp), _from(from), _to(to), _subject(subject) {}

    int compareByTime(const CTimeStamp &x) const {
        return _timeStamp.compare(x);
    }

    int compareByTime(const CMail &x) const {
        return _timeStamp.compare(x._timeStamp);
    }

    const std::string &from() const {
        return _from;
    }

    const std::string &to() const {
        return _to;
    }

    const std::optional<std::string> &subject() const {
        return _subject;
    }

    const CTimeStamp &timeStamp() const {
        return _timeStamp;
    }

    friend std::ostream &operator<<(std::ostream &os, const CMail &x) {
        os << x._timeStamp << " " << x._from << " -> " << x._to;
        if (x._subject) {
            os << ", subject: " << *x._subject;
        }
        return os;
    }

private:
    CTimeStamp _timeStamp;
    std::string _from, _to;
    std::optional<std::string> _subject;
};

// your code will be compiled in a separate namespace
namespace MysteriousNamespace {

#endif /* __PROGTEST__ */

//----------------------------------------------------------------------------------------
class CMailLog {
public:
    int parseLog(std::istream &in) {
        int count = 0;
        std::string line;
        while (std::getline(in, line)) {
            std::istringstream lineStream(line);
            count += parseLine(lineStream);
        }
        std::ranges::for_each(sentMail, [](auto &a) { a.second.processed = true; });
        return count;
    }

    std::list<CMail> listMail(const CTimeStamp &from, const CTimeStamp &to) const {

        auto it_from = receivedMail.lower_bound(CMail(from, "", "", std::nullopt));
        auto it_to = receivedMail.upper_bound(CMail(to, "", "", std::nullopt));

        return {it_from,it_to};
    }

    std::set<std::string> activeUsers(const CTimeStamp &from, const CTimeStamp &to) const {
        std::set<std::string> result;
        const auto it_from = receivedMail.lower_bound(CMail(from, "", "", std::nullopt));
        const auto it_to = receivedMail.upper_bound(CMail(to, "", "", std::nullopt));
        for_each(it_from, it_to, [&result](const CMail &mail) {
            result.insert(mail.to());
            result.insert(mail.from());
        });

        return result;
    }

private:
    struct CMailCmp {
        bool operator()(const CMail &x, const CMail &y) const {
            return x.compareByTime(y) < 0;
        }
    };
    struct MetaData {
        std::string sender;
        std::optional<std::string> subject;
        bool processed = false;
    };
    std::unordered_map<std::string, MetaData> sentMail;
    std::multiset<CMail, CMailCmp> receivedMail;

    int insertLog(const std::string & rest, const CTimeStamp &timeStamp, const std::string &id) {
        if (rest.rfind("from=", 0) == 0) {
            if (!sentMail.contains(id)) {
                sentMail[id] = MetaData(rest.substr(5), std::nullopt);
            }
        }
        if (rest.rfind("subject=", 0) == 0) {
            if (sentMail.contains(id) && sentMail[id].processed == false) {
                sentMail[id].subject = rest.substr(8);
            }
        }
        if (rest.rfind("to=", 0) == 0) {
            if (sentMail.contains(id) && sentMail[id].processed == false) {
                receivedMail.insert(CMail(timeStamp, sentMail[id].sender, rest.substr(3), sentMail[id].subject));
                return 1;
            }

        }
        return 0;
    }

    int parseLine(std::istream & lineStream) {
        std::string relay, id, rest, month;
        int year, day, hour, minute;
        char discard;
        double sec;

        if (!(lineStream >> month >> day >> year >> hour >> discard >> minute >> discard >> sec >> relay >> id))
            return false;

        lineStream >> std::ws;
        std::getline(lineStream, rest);

        id.pop_back();

        if(!monthList.contains(month)) return false;
        CTimeStamp timeStamp(year,monthList[month] , day, hour, minute, sec);


        return insertLog(rest, timeStamp, id);
    }

    std::map<std::string, int> monthList = {
        {"Jan", 1}, {"Feb", 2}, {"Mar", 3}, {"Apr", 4},
        {"May", 5}, {"Jun", 6}, {"Jul", 7}, {"Aug", 8},
        {"Sep", 9}, {"Oct", 10}, {"Nov", 11}, {"Dec", 12}
    };
};
//----------------------------------------------------------------------------------------
#ifndef __PROGTEST__
} // namespace
std::string             printMail       ( const std::list<CMail> & all )
{
  std::ostringstream oss;
  for ( const auto & mail : all )
    oss << mail << "\n";
  return oss . str ();
}
int                     main ()
{
  MysteriousNamespace::CMailLog m;
  std::list<CMail> mailList;
  std::set<std::string> users;
  std::istringstream iss;

  iss . clear ();
  iss . str (
    "Mar 29 2025 12:35:32.233 relay.fit.cvut.cz ADFger72343D: from=user1@fit.cvut.cz\n"
    "Mar 29 2025 12:37:16.234 relay.fit.cvut.cz JlMSRW4232Df: from=person3@fit.cvut.cz\n"
    "Mar 29 2025 12:55:13.023 relay.fit.cvut.cz JlMSRW4232Df: subject=New progtest homework!\n"
    "Mar 29 2025 13:38:45.043 relay.fit.cvut.cz Kbced342sdgA: from=office13@fit.cvut.cz\n"
    "Mar 29 2025 13:36:13.023 relay.fit.cvut.cz JlMSRW4232Df: to=user76@fit.cvut.cz\n"
    "Mar 29 2025 13:55:31.456 relay.fit.cvut.cz KhdfEjkl247D: from=PR-department@fit.cvut.cz\n"
    "Mar 29 2025 14:18:12.654 relay.fit.cvut.cz Kbced342sdgA: to=boss13@fit.cvut.cz\n"
    "Mar 29 2025 14:48:32.563 relay.fit.cvut.cz KhdfEjkl247D: subject=Business partner\n"
    "Mar 29 2025 14:58:32.000 relay.fit.cvut.cz KhdfEjkl247D: to=HR-department@fit.cvut.cz\n"
    "Mar 29 2025 14:25:23.233 relay.fit.cvut.cz ADFger72343D: mail undeliverable\n"
    "Mar 29 2025 15:02:34.231 relay.fit.cvut.cz KhdfEjkl247D: to=CIO@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=CEO@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=dean@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=vice-dean@fit.cvut.cz\n"
    "Mar 29 2025 15:02:34.230 relay.fit.cvut.cz KhdfEjkl247D: to=archive@fit.cvut.cz\n" );
  assert ( m . parseLog ( iss ) == 8 );
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 23, 59, 59 ) );
  assert ( printMail ( mailList ) == R"###(2025-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!
2025-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
2025-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> CEO@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> dean@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> vice-dean@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.230 PR-department@fit.cvut.cz -> archive@fit.cvut.cz, subject: Business partner
2025-03-29 15:02:34.231 PR-department@fit.cvut.cz -> CIO@fit.cvut.cz, subject: Business partner
)###" );
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 14, 58, 32 ) );
  assert ( printMail ( mailList ) == R"###(2025-03-29 13:36:13.023 person3@fit.cvut.cz -> user76@fit.cvut.cz, subject: New progtest homework!
2025-03-29 14:18:12.654 office13@fit.cvut.cz -> boss13@fit.cvut.cz
2025-03-29 14:58:32.000 PR-department@fit.cvut.cz -> HR-department@fit.cvut.cz, subject: Business partner
)###" );
  mailList = m . listMail ( CTimeStamp ( 2025, 3, 30, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 30, 23, 59, 59 ) );
  assert ( printMail ( mailList ) == "" );
  users = m . activeUsers ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 23, 59, 59 ) );
  assert ( users == std::set<std::string>( { "CEO@fit.cvut.cz", "CIO@fit.cvut.cz", "HR-department@fit.cvut.cz", "PR-department@fit.cvut.cz", "archive@fit.cvut.cz", "boss13@fit.cvut.cz", "dean@fit.cvut.cz", "office13@fit.cvut.cz", "person3@fit.cvut.cz", "user76@fit.cvut.cz", "vice-dean@fit.cvut.cz" } ) );
  users = m . activeUsers ( CTimeStamp ( 2025, 3, 28, 0, 0, 0 ),
                            CTimeStamp ( 2025, 3, 29, 13, 59, 59 ) );
  assert ( users == std::set<std::string>( { "person3@fit.cvut.cz", "user76@fit.cvut.cz" } ) );
  return EXIT_SUCCESS;
}
#endif /* __PROGTEST__ */
