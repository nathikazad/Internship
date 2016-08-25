/*
 * SDdriver.H
 *
 * This is a wrapper to Arduino SD Libraries to create files and write to them easily
 *
 */
#ifndef _SDDRIVER_H_
#define _SDDRIVER_H_

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>


class SDdriver {
public:
    // maximum number of timers

    SDdriver(int CS, boolean debug);
    boolean initialize();
    void create_file();
    void flush();
    void close();

    void println();

    void print(char* character_array);
    void println(char* character_array);

    void print(String character_array);
    void println(String character_array);

    void print(char character);
    void println(char character);

    void print_int(int number);
    void println_int(int number);

    void print_long(long number);
    void println_long(long number);

    void print_long_long(long long number);
    void println_long_long(long long number);

private:
	File logfile;
	int CS;
	boolean debug;
};
 #endif // _SDDRIVER_H_