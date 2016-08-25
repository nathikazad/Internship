#ifndef internal_RTC_clock_h
#define internal_RTC_clock_h

#include "Arduino.h"
#include "DateNTime.h"
// Includes Atmel CMSIS
#include <chip.h>


#define RC							0
#define	XTAL						1

class Internal_RTC_clock
{
	public:
		Internal_RTC_clock (int source);
		void init ();
		void set_dateNtime(DateNTime dateNtime);
		DateNTime get_dateNtime();

		void set_alarmtime (int hour, int minute, int second);
		void set_alarmdate (int day, int month);
		
		void attachalarm (void (*)(void));
		void disable_alarm ();
	private:

		// DateNTime _dateNtime;

		int get_hours ();
		int get_minutes ();
		int get_seconds ();
		uint16_t get_years ();
		int get_months ();
		int get_days ();
		int get_day_of_week ();
		int set_hours (int _hour);
		int set_minutes (int minute);
		int set_seconds (int second);
		int set_days (int day);
		int set_months (int month);
		int set_years (uint16_t year);

		DateNTime _dateNtime;
		int _source;
		int _hour;
		int _minute;
		int _second;
		int _day;
		int _month;
		uint16_t _year;
		int _day_of_week;
		int calculate_day_of_week (uint16_t _year, int _month, int _day);
		uint32_t current_time ();
		uint32_t current_date ();
		uint32_t _current_time;
		uint32_t _current_date;
		uint32_t change_time (uint32_t _now);
		uint32_t change_date (uint32_t _now);
		uint32_t _now;
		uint32_t _changed;
};

#endif