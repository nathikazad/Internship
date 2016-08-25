#ifndef DesertStar_h
#define DesertStar_h

#include "Arduino.h"
#include "String.h"
#include "SDdriver.h"

class DesertStar
{
  public:
    DesertStar(char* _address,  HardwareSerial* _rs485_serial_port, boolean _debug, int _tx_control, SDdriver* _memory_card=NULL);

    void set_as_receiver(String threshold, boolean ext_clock_present);
    void set_as_pinger(boolean ext_clock_present);
    void reset_sync_counter();
    
    void ask_last_ping();
    boolean last_ping_received();
    void print_last_buffer();
    long long get_ping_difference();
    long long get_time_of_last_ping();

  private:

    void set_sync_period(String period);
    void set_blackout_period(String period);
    void print(String output);
    void print(char* output);
    void println(String output);
    void println(char* output);
    void do_till_acknowledgment_received(String deed_name, String command);
    boolean acknowledged();
    boolean newline_received_after_checking_for_new_character();
    void set_threshold(String value);
    void set_sync_mode_external_clock();
    void set_sync_mode_internal_clock();
    void turn_pinger_off();
    void turn_pinger_on();
    void turn_receiver_on();
    void turn_receiver_off();
    void set_receiver_synchronous_reception();
    void set_tx_delay(String value);
    void extract_time_information_and_lines_received_statistics();
    long long char2LL(char *str);

  	String address;
  	HardwareSerial* rs485_serial_port;
    boolean debug;
    SDdriver* memory_card;
    int tx_control;

    String RS485_read_buff="";
    char RS485_read_buff_char[100];
    int RS485_read_buff_char_index=0;
    String RS485_last_buff;
    long long last_ping_time=0;
    long long difference=0;

};

#endif