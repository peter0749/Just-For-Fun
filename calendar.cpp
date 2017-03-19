#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <vector>
//#undef __linux__

namespace Calendar {
    const int MONTHLIST[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const std::string MONTH_NAME[] = { 
        "", // for 1-base
        "January"   ,"February" ,"March",
        "April"     ,"May"      ,"June",
        "July"      ,"August"   ,"September",
        "October"   ,"November" ,"December"
    };
    const std::string WEEK[] = {
        "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
    };
    class Calendar1901 {
        private:
            int WORD_LEN;
            int WIDTH;
            inline bool inputYrMn(int year, int month) {
                //judge whether the input is valid.
                if(year > 2099 || year < 1901)  return false;
                if(month < 1 || month > 12) return false;
                return true;
            }

            inline bool isLeap(int year ) {
                if(year % 4) return false;
                if(year % 100) return true;
                if(year % 400) return false;
                return true;
            }

            void get1stOfMonth(int year, int month, int &firstDay, int &theNumberOfDays) {
                // gets the value of firstDay.
                int century = year / 100;
                int lowerYear = year%100;
                bool monLt(month<=2);
                if(monLt) --lowerYear;
                firstDay = lowerYear + lowerYear/4 + century/4 - 2*century + \
                           ((26*( (monLt?month+12:month ) + 1))/10);
                firstDay = ((firstDay % 7) + 7) % 7;
                // gets the value of theNumberOfDays.
                theNumberOfDays = MONTHLIST[month];
                if(month == 2 && isLeap(year)) {
                    ++theNumberOfDays;
                }
            }
#ifdef __linux__
            inline void setfontGreen(void) { std::cout << "\033[32m"; }
            inline void setfontMagenta(void) { std::cout << "\033[35m"; }
            inline void setfontCyan(void) { std::cout << "\033[36m"; }
            inline void setfontRed(void) { std::cout << "\033[31m"; }
            inline void setfontDefault(void) { std::cout << "\033[0m"; }
            inline void setfontBold(void) { std::cout << "\033[1m"; }
#endif
            void getCalendar(int year, int month, int firstDay, int theNumberOfDays) {
                //std::setw(4): aligned 4 characters as a group
#ifdef __linux__
                setfontGreen();
#endif
                std::cout << std::setfill('-') << std::setw(WIDTH) << "" << std::endl;
#ifdef __linux__
                setfontDefault(); setfontBold();
#endif
                std::cout << std::setfill(' ') << std::setw(WORD_LEN*3) << year \
                    << std::setw(WORD_LEN) << std::setw(0) << ' ' << MONTH_NAME[month] << std::endl;
#ifdef __linux__
                setfontDefault(); setfontGreen();
#endif
                std::cout << std::setfill('-') << std::setw(WIDTH) << "" << std::endl;
#ifdef __linux__
                setfontDefault();
#endif
                std::cout << std::setfill(' ');
#ifdef __linux__
                setfontCyan();
#endif
                for(int i=0; i<7; ++i)
                    std::cout << std::setw(WORD_LEN) << WEEK[i];
#ifdef __linux__
                setfontDefault();
#endif
                std::cout << std::endl;
                std::cout << std::setw(WORD_LEN*firstDay) << "" ;
                int Day(1);
                for(int i=firstDay; i<7; ++i, ++Day) {
                    int t=i%7;
#ifdef __linux__
                    if(t==0||t==6) {
                        setfontRed();
                    } else setfontDefault();
#endif
                    std::cout << std::setw(WORD_LEN) << Day;
                }
                for(int i=0; Day<=theNumberOfDays; ++Day, ++i) {
                    int t=i%7;
#ifdef __linux__
                    if(t==0||t==6) {
                        setfontRed();
                    } else setfontDefault();
#endif
                    if(t==0) std::cout<<std::endl;
                    std::cout << std::setw(WORD_LEN) << Day;
                }
                std::cout << std::endl;
#ifdef __linux__
                setfontGreen();
#endif
                std::cout << std::setfill('-') << std::setw(WIDTH) << "" << std::endl;
#ifdef __linux__
                setfontDefault();
#endif
            }
        public:
            inline void adjW(int l=4) {
                this->WORD_LEN=(l<4?4:l);
                this->WIDTH=this->WORD_LEN*7;
            }
            Calendar1901(int l=4) {
                this->WORD_LEN=(l<4?4:l);
                this->WIDTH=this->WORD_LEN*7;
            }
            bool showCalendar(int year, int month) {
                int firstDay, theNumberOfDays;
                if(!inputYrMn(year, month)) return false;
                get1stOfMonth(year, month, firstDay, theNumberOfDays);
                getCalendar(year, month, firstDay, theNumberOfDays);
                return true;
            }
    };
};

using namespace std;

int main(void) {
    int year, month, wsize;
    string checkExit;
    Calendar::Calendar1901 cla(6);
    while(1) {
        try {
            cout << "Enter\n" << "year month width" << endl;
            cin >> year >> month >> wsize;
            cla.adjW(wsize);
            if(!cin.good() || !cla.showCalendar(year, month)) throw runtime_error("Invalid year/month");
        } catch(runtime_error err) {
            cerr<<err.what()<<endl;
            cin.clear();
            cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
        }
        cout << "Enter y/Y to continue" << endl;
        cin>>checkExit;
        if( !cin.good() || !(checkExit=="y" || checkExit=="Y")) break;
    }
    return 0;    
}
