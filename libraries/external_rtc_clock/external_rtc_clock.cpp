#include "external_rtc_clock.h"

External_RTC_clock::External_RTC_clock(int chipSelect)
{
	_chipSelect=chipSelect;
}

void External_RTC_clock::init()
{
	pinMode(_chipSelect,OUTPUT); // chip select
    // start the SPI library:
    SPI.begin();
    SPI.setBitOrder(MSBFIRST); 
    SPI.setDataMode(SPI_MODE1); // both mode 1 & 3 should work 
    //set control register 
    digitalWrite(_chipSelect, LOW);  
    SPI.transfer(0x8E);
    SPI.transfer(0x60); //60= disable Osciallator and Battery SQ wave @1hz, temp compensation, Alarms disabled
    digitalWrite(_chipSelect, HIGH);
    delay(10);
}

void External_RTC_clock::set_dateNtime(DateNTime dateNtime)
{
	_dateNtime=dateNtime;
	int TimeDate [7]={_dateNtime.get_second(),_dateNtime.get_minute(),_dateNtime.get_hour(),0,
		_dateNtime.get_day(),_dateNtime.get_month(),_dateNtime.get_year()};
	for(int i=0; i<=6;i++){
	    if(i==3)
	      i++;
	    int b= TimeDate[i]/10;
	    int a= TimeDate[i]-b*10;
	    if(i==2){
	      if (b==2)
	        b=B00000010;
	      else if (b==1)
	        b=B00000001;
    	} 
	    TimeDate[i]= a+(b<<4);

	    SPI.setBitOrder(MSBFIRST); 
	    SPI.setDataMode(SPI_MODE1); // both mode 1 & 3 should work 
	    digitalWrite(_chipSelect, LOW);
	    SPI.transfer(i+0x80); 
	    SPI.transfer(TimeDate[i]);        
	    digitalWrite(_chipSelect, HIGH);
	}
}

DateNTime External_RTC_clock::get_dateNtime()
{
  	int TimeDate [7]; //second,minute,hour,null,day,month,year    
  	for(int i=0; i<=6;i++){
	    	if(i==3)
	      i++;

	    SPI.setBitOrder(MSBFIRST); 
	    SPI.setDataMode(SPI_MODE1); // both mode 1 & 3 should work 
	    digitalWrite(_chipSelect, LOW);
	    SPI.transfer(i+0x00); 
	    unsigned int n = SPI.transfer(0x00);        
	    digitalWrite(_chipSelect, HIGH);
	    int a=n & B00001111;    
	    if(i==2){ 
	    	int b=(n & B00110000)>>4; //24 hour mode
	    	if(b==B00000010)
	        	b=20;        
	      	else if(b==B00000001)
	        	b=10;
	      TimeDate[i]=a+b;
	    }
	    else if(i==4){
	    	int b=(n & B00110000)>>4;
	      	TimeDate[i]=a+b*10;
	    }
	    else if(i==5){
	    	int b=(n & B00010000)>>4;
	    	TimeDate[i]=a+b*10;
	    }
	    else if(i==6){
	    	int b=(n & B11110000)>>4;
	    	TimeDate[i]=a+b*10;
	    }
	    else{ 
	    	int b=(n & B01110000)>>4;
	    	TimeDate[i]=a+b*10; 
		}
	}
  //second,minute,hour,null,day,month,year 
	_dateNtime.set_second(TimeDate[0]);
	_dateNtime.set_minute(TimeDate[1]);
	_dateNtime.set_hour(TimeDate[2]);
	_dateNtime.set_day(TimeDate[4]);
	_dateNtime.set_month(TimeDate[5]);
	_dateNtime.set_year(TimeDate[6]);
	return _dateNtime;
}