#include "Arduino.h"
#include "DateNTime.h"

DateNTime::DateNTime(int day, int month, int year, int hour, int minute, int second)
{
	_date_time.second = second;
	_date_time.minute = minute;
	_date_time.hour   = hour;
	_date_time.day    = day;
	_date_time.month  = month;
	_date_time.year   = year;
	update_epoch();
}

DateNTime::DateNTime(unsigned long epoch)
{
	_epoch=epoch;
	update_date_time();
}

DateNTime::DateNTime(date_time dt)
{
	_date_time=dt;
	update_epoch();
}

int DateNTime::get_hour()
{
	return _date_time.hour;
}

int DateNTime::get_minute()
{
	return _date_time.minute;
}

int DateNTime::get_second()
{
	return _date_time.second;
}

int DateNTime::get_year()
{
	return _date_time.year;
}	

int DateNTime::get_month()
{
	return _date_time.month;
}

int DateNTime::get_day()
{
	return _date_time.day;
}

unsigned long DateNTime::get_epoch()
{
	return _epoch;
}

void DateNTime::set_hour(unsigned int hour)
{
	_date_time.hour = hour;
	update_epoch();
}

void DateNTime::set_minute(unsigned int minute)
{
	_date_time.minute = minute;
	update_epoch();
}

void DateNTime::set_second(unsigned int second)
{
	_date_time.second = second;
	update_epoch();
}

void DateNTime::set_year(unsigned int year)
{
	_date_time.year = year;
	update_epoch();
}

void DateNTime::set_month(unsigned int month)
{
	_date_time.month = month;
	update_epoch();
}

void DateNTime::set_day(unsigned int day)
{
	_date_time.day = day;
	update_epoch();
}

void DateNTime::set_epoch(unsigned long epoch)
{
	_epoch = epoch;
	update_date_time();
}

void DateNTime::print_epoch()
{
	Serial.println(_epoch);
}

void DateNTime::print_date_time()
{
	Serial.print(_date_time.month);
	Serial.print("/");
	Serial.print(_date_time.day);
	Serial.print("/");
	Serial.print(_date_time.year);
	Serial.print(" ");
	Serial.print(_date_time.hour);
	Serial.print(":");
	Serial.print(_date_time.minute);
	Serial.print(":");
	Serial.println(_date_time.second);
}

void DateNTime::update_epoch()
{
	unsigned int second = _date_time.second;  // 0-59
    unsigned int minute = _date_time.minute;  // 0-59
    unsigned int hour   = _date_time.hour;    // 0-23
    unsigned int day    = _date_time.day-1;   // 0-30
    unsigned int month  = _date_time.month-1; // 0-11
    unsigned int year   = _date_time.year+30;    // 0-99
    _epoch = (((year/4*(365*4+1)+days[year%4][month]+day)*24+hour)*60+minute)*60+second;
}


void DateNTime::update_date_time()
{
	unsigned long epoch_dup=_epoch;
	_date_time.second = epoch_dup%60; 
	epoch_dup /= 60;
    _date_time.minute = epoch_dup%60; 
    epoch_dup /= 60;
    _date_time.hour   = epoch_dup%24;
     epoch_dup /= 24;

    unsigned int years = epoch_dup/(365*4+1)*4; 
    epoch_dup %= 365*4+1;

    unsigned int year;
    for (year=3; year>0; year--)
    {
        if (epoch_dup >= days[year][0])
            break;
    }

    unsigned int month;
    for (month=11; month>0; month--)
    {
        if (epoch_dup >= days[year][month])
            break;
    }

    _date_time.year  = years+year;
    _date_time.month = month+1;
    _date_time.day   = epoch_dup-days[year][month]+1;
}

bool DateNTime::operator>(const DateNTime& other) {
  return _epoch > other._epoch;
}
bool DateNTime::operator<(const DateNTime& other) {
  return _epoch < other._epoch;
}

DateNTime DateNTime::operator+(const unsigned long& other) {
  	DateNTime result(_epoch+other);
  	return result;
}
