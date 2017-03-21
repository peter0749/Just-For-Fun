#include <iostream>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <limits>
#include <vector>
#include <cstring>
#ifdef __APPLE__
#define ___USECOL___
#endif
#ifdef __linux__
#define ___USECOL___
#endif

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
                bool monLt(month<=2);
                int century   = (monLt?year-1:year)/100;
                int lowerYear = (monLt?year-1:year)%100;
                firstDay = lowerYear + lowerYear/4 + century/4 - 2*century + \
                           ((26*( (monLt?month+12:month ) + 1))/10);
                firstDay = ((firstDay % 7) + 7) % 7;
                // gets the value of theNumberOfDays.
                theNumberOfDays = MONTHLIST[month];
                if(month == 2 && isLeap(year)) {
                    ++theNumberOfDays;
                }
            }

#ifdef ___USECOL___
            inline void setfontGreen(void) { std::cout << "\033[32m"; }
            inline void setfontMagenta(void) { std::cout << "\033[35m"; }
            inline void setfontCyan(void) { std::cout << "\033[36m"; }
            inline void setfontRed(void) { std::cout << "\033[31m"; }
            inline void setfontDefault(void) { std::cout << "\033[0m"; }
            inline void setfontBold(void) { std::cout << "\033[1m"; }
#endif
            void getCalendar(int year, int month, int firstDay, int theNumberOfDays, int day) {
                //std::setw(4): aligned 4 characters as a group
#ifdef ___USECOL___
                setfontGreen();
#endif
                std::cout << std::setfill('=') << std::setw(WIDTH) << "" << std::endl;
#ifdef ___USECOL___
                setfontDefault(); setfontBold();
#endif
                std::cout << std::setfill(' ') << std::setw(WORD_LEN*3) << year \
                    << std::setw(WORD_LEN) << std::setw(0) << ' ' << MONTH_NAME[month] << std::endl;
#ifdef ___USECOL___
                setfontDefault(); setfontGreen();
#endif
                std::cout << std::setfill('-') << std::setw(WIDTH) << "" << std::endl;
#ifdef ___USECOL___
                setfontDefault();
#endif
                std::cout << std::setfill(' ');
#ifdef ___USECOL___
                setfontCyan();
#endif
                for(int i=0; i<7; ++i)
                    std::cout << std::setw(WORD_LEN) << WEEK[i];
#ifdef ___USECOL___
                setfontDefault();
#endif
                std::cout << std::endl;
                std::cout << std::setw(WORD_LEN*firstDay) << "" ;
                int Day(1);
                for(int i=firstDay; i<7; ++i, ++Day) {
                    int t=i%7;
#ifdef ___USECOL___
                    if(Day==day) {
                        setfontCyan();
                    } else if(t==0||t==6) {
                        setfontRed();
                    } else setfontDefault();
#endif
                    std::cout << std::setw(WORD_LEN) << Day;
                }
                for(int i=0; Day<=theNumberOfDays; ++Day, ++i) {
                    int t=i%7;
#ifdef ___USECOL___
                    if(Day==day) {
                        setfontCyan();
                    } else if(t==0||t==6) {
                        setfontRed();
                    } else setfontDefault();
#endif
                    if(t==0) std::cout<<std::endl;
                    std::cout << std::setw(WORD_LEN) << Day;
                }
                std::cout << std::endl;
#ifdef ___USECOL___
                setfontGreen();
#endif
                std::cout << std::setfill('=') << std::setw(WIDTH) << "" << std::endl;
#ifdef ___USECOL___
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
            bool showCalendar(int year, int month, int day) {
                int firstDay, theNumberOfDays;
                if(!inputYrMn(year, month)) return false;
                get1stOfMonth(year, month, firstDay, theNumberOfDays);
                getCalendar(year, month, firstDay, theNumberOfDays, day);
                return true;
            }
    };
};

using namespace std;

int main(int argc, char **argv) {
    if(argc<4) return -1;
    Calendar::Calendar1901 cal(argc==5?atoi(argv[4]):4);
    if(!cal.showCalendar(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]))) {
        cerr<<"Invalid input format!"<<endl;
        cerr<<"Note: year[1901,2099], month[1-12]"<<endl;
    }
    return 0;    
}
