#ifndef external_RTC_clock_h
#define external_RTC_clock_h

#include "Arduino.h"
#include <SPI.h>
#include "DateNTime.h"




class External_RTC_clock
{

  public:
  	External_RTC_clock(int cs);
    void init();
 	
	void set_dateNtime(DateNTime dateNtime);
	DateNTime get_dateNtime();


  private:

  int _chipSelect;
  DateNTime _dateNtime;

};

#endif