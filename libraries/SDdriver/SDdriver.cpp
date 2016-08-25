#include "SDdriver.h"

SDdriver::SDdriver(int _CS, boolean _debug) {
	CS=_CS;
	debug=_debug;
}

boolean SDdriver::initialize()
{
	if(debug)
		Serial.print("Initializing SD card...");
	pinMode(CS, OUTPUT);
	if (!SD.begin(CS)) {
		if(debug)
			Serial.println("Card init. failed!");
		return false;
  	}
  	if(debug)
		Serial.println("card initialized.");
	return true;
}
void SDdriver::create_file()
{
	char filename[19];
	int trial_number=0;
	do{
		sprintf(filename,"trial%03d.txt",trial_number);
		trial_number++;
	}while(SD.exists(filename));
	logfile = SD.open(filename, FILE_WRITE); 
	if(!logfile && debug) {
		Serial.print("Couldnt create ");
		Serial.println(filename);
	}
	else if(debug)
	{
	  Serial.print("Logging to: ");
	  Serial.println(filename);
	}

}
void SDdriver::flush()
{
	logfile.flush();
}

void SDdriver::close()
{
	logfile.close();
}


void SDdriver::println()
{
	if(debug)
		Serial.println();
  	logfile.write('\n');
}

void SDdriver::print(char* character_array)
{
	if(debug)
		Serial.print(character_array);
  	logfile.write(character_array);
}

void SDdriver::println(char* character_array)
{
	if(debug)
		Serial.println(character_array);
  	logfile.write(character_array);
  	logfile.write('\n');
}

void SDdriver::print(String character_array)
{
	
	if(debug)
		Serial.print(character_array);
	char buff[character_array.length()];
	character_array.toCharArray(buff,character_array.length());
  	logfile.write(buff);
}
void SDdriver::println(String character_array)
{
	if(debug)
		Serial.println(character_array);
	char buff[character_array.length()];
  	character_array.toCharArray(buff,character_array.length());
  	logfile.write(buff);
  	logfile.write('\n');
}

void SDdriver::print(char character)
{
	if(debug)
		Serial.print(character);
  	logfile.write(character);
}
void SDdriver::println(char character)
{
	if(debug)
		Serial.println(character);
  	logfile.write(character);
  	logfile.write('\n');
}

void SDdriver::print_int(int number)
{
	if(debug)
		Serial.print(number);
  	logfile.write(number);
}
    
void SDdriver::println_int(int number)
{
	if(debug)
		Serial.println(number);
  	logfile.write(number);
  	logfile.write('\n');
}

void SDdriver::print_long(long number)
{
	if(debug)
		Serial.print(number);
  	logfile.write(number);
}

void SDdriver::println_long(long number)
{
	if(debug)
		Serial.println(number);
  	logfile.write(number);
  	logfile.write('\n');
}

void SDdriver::print_long_long(long long number)
{
	uint64_t xx = number/1000000000ULL;
  	if (xx >0) 
  	{
    	if(debug)
			Serial.print((long)xx);
    	logfile.write((long)xx);
  	}
  	if(debug)
		Serial.print((long)(number-xx*1000000000));
  	logfile.write((long)(number-xx*1000000000));
}

void SDdriver::println_long_long(long long number)
{
	uint64_t xx = number/1000000000ULL;
  	if (xx >0) 
  	{
    	if(debug)
			Serial.print((long)xx);
    	logfile.write((long)xx);
  	}
  	if(debug)
		Serial.println((long)(number-xx*1000000000));
  	logfile.write((long)(number-xx*1000000000));
  	logfile.write('\n');
}
