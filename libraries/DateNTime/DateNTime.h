#ifndef DateNTime_h
#define DateNTime_h

#include "Arduino.h"
#include "String.h"

typedef struct{
    unsigned int second; // 0-59
    unsigned int minute; // 0-59
    unsigned int hour;   // 0-23
    unsigned int day;    // 1-31
    unsigned int month;  // 1-12
    unsigned int year;   // 0-99 (representing 2000-2099)
}date_time;

static unsigned short days[4][12] =
{
    {   0,  31,  60,  91, 121, 152, 182, 213, 244, 274, 305, 335},
    { 366, 397, 425, 456, 486, 517, 547, 578, 609, 639, 670, 700},
    { 731, 762, 790, 821, 851, 882, 912, 943, 974,1004,1035,1065},
    {1096,1127,1155,1186,1216,1247,1277,1308,1339,1369,1400,1430},
};


class DateNTime
{

  public:
  	DateNTime(int day, int month, int year, int hour, int minute, int seconds);
  	DateNTime(unsigned long epoch=0);
  	DateNTime(date_time dt);

   	int get_hour();
  	int get_minute();
  	int get_second();
  	int get_year();
  	int get_month();
  	int get_day();
    unsigned long get_epoch();

  	void set_hour(unsigned int);
  	void set_minute(unsigned int);
  	void set_second(unsigned int);
  	void set_year(unsigned int);
  	void set_month(unsigned int);
  	void set_day(unsigned int);
    void set_epoch(unsigned long);

    void print_date_time();
    void print_epoch();

    bool operator>(const DateNTime& other);
    bool operator<(const DateNTime& other);
    DateNTime operator+(const unsigned long& other);
    // DateNTime operator-(const DateNTime& other);
  private:
  	void update_epoch();
  	void update_date_time();

  date_time _date_time;
  unsigned long _epoch;



};

#endif

