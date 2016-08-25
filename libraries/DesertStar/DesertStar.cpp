
#include "Arduino.h"
#include "DesertStar.h"


DesertStar::DesertStar(char* _address,  HardwareSerial* _rs485_serial_port, boolean _debug, 
              int _tx_control, SDdriver* _memory_card)
{
  int i=0;
  address="";
  while(_address[i]!=0)
  {
    address+=_address[i];
    i++;
  }
  rs485_serial_port=_rs485_serial_port;
  debug=_debug;
  memory_card=_memory_card;
  tx_control=_tx_control;
}

// Range for threshold 0 to 140
void DesertStar::set_as_receiver(String threshold, boolean ext_clock_present) 
{
  pinMode(tx_control, OUTPUT);  
  println("Initializing DSM receiver");
  set_sync_period("1000000");
  set_blackout_period("900000");
  set_threshold(threshold);
  if(ext_clock_present)
    set_sync_mode_external_clock();
  else
    set_sync_mode_internal_clock();
  turn_pinger_off();
  turn_receiver_on();
  set_receiver_synchronous_reception();
}

void DesertStar::set_as_pinger(boolean ext_clock_present)
{
  
  println("Initializing DSM pinger");
  turn_pinger_on();
  turn_receiver_off();
  reset_sync_counter();
  if(ext_clock_present)
    set_sync_mode_external_clock();
  else
    set_sync_mode_internal_clock();
  set_tx_delay("10000");
}

void DesertStar::ask_last_ping()
{
  digitalWrite(tx_control, HIGH);  // Enable RS485 Transmit   
  rs485_serial_port->println("$"+address+",0,AD,P");
  delay(10);
  digitalWrite(tx_control, LOW);  // Disable RS485 Transmit   
}

void DesertStar::set_sync_period(String period)
{
  do_till_acknowledgment_received("Setting Sync Period","$"+address+",0,ST,SYNCPRD,"+period);
}

void DesertStar::set_blackout_period(String period)
{
  do_till_acknowledgment_received("Setting Blackout Period","$"+address+",0,ST,BLKOUT,"+period);
}

void DesertStar::set_threshold(String value)
{
  do_till_acknowledgment_received("Setting Threshold","$"+address+",0,ST,DT,"+value);
}

void DesertStar::set_sync_mode_internal_clock()
{
  do_till_acknowledgment_received("Setting Sync to internal clock","$"+address+",0,AS,3");
}

void DesertStar::set_sync_mode_external_clock()
{
  do_till_acknowledgment_received("Setting Sync to EXTERNAL clock","$"+address+",0,AS,2");
}

void DesertStar::reset_sync_counter()
{
  do_till_acknowledgment_received("Resetting sync counter","$"+address+",0,AS,1");
}

void DesertStar::turn_pinger_off()
{
  do_till_acknowledgment_received("Turning Pinger off","$"+address+",0,AT,0");
}

void DesertStar::turn_pinger_on()
{
  do_till_acknowledgment_received("Turning Pinger on","$"+address+",0,AT,1");
}

void DesertStar::turn_receiver_off()
{
  do_till_acknowledgment_received("Turning Receiver off","$"+address+",0,AR,0");
}

void DesertStar::turn_receiver_on()
{
  do_till_acknowledgment_received("Turning Receiver on","$"+address+",0,AR,1");
}

void DesertStar::set_receiver_synchronous_reception()
{
  do_till_acknowledgment_received("Setting synchronous reception","$"+address+",0,AR,2,0");
}

void DesertStar::set_tx_delay(String value)
{
  do_till_acknowledgment_received("Setting tx delay","$"+address+",0,TXDELAY,"+value);
}

void DesertStar::do_till_acknowledgment_received(String deed_name, String command)
{  

  print("\tAttempting ");
  print(deed_name);
  do
  {
    print(".");
    digitalWrite(tx_control, HIGH);  // Enable RS485 Transmit   
    rs485_serial_port->println(command); 
    delay(10);
    digitalWrite(tx_control, LOW);  // Disable RS485 Transmit    
  }
  while(!acknowledged());
  println("Success");
}

boolean DesertStar::acknowledged()
{
  int start_time=millis();
  RS485_read_buff="";
  while(millis()-start_time<1000)
  {
    if(newline_received_after_checking_for_new_character())
      return RS485_last_buff.startsWith("$0,"+address+",OK");
  }
  return false;
}

boolean DesertStar::newline_received_after_checking_for_new_character()
{
  while (rs485_serial_port->available()) //USED TO BE IF
  {
    char character=rs485_serial_port->read();
    RS485_read_buff+=character;
    RS485_read_buff_char[RS485_read_buff_char_index]=character;
    RS485_read_buff_char_index++;
    if(character=='\n')
    {
      RS485_read_buff_char[RS485_read_buff_char_index]=0;
      int offset_error=0;
      while(RS485_read_buff_char[offset_error]!='$')
        offset_error++;
      RS485_last_buff=String(RS485_read_buff_char+offset_error);
      RS485_read_buff_char_index=0;
      RS485_read_buff="";
      return true;
    }      
  }
  return false;
}

boolean DesertStar::last_ping_received()
{
  if(newline_received_after_checking_for_new_character() && RS485_last_buff.startsWith("$0,"+address+",DP"))
  {
    extract_time_information_and_lines_received_statistics();
    return true;
  }
  return false;
}

void  DesertStar::extract_time_information_and_lines_received_statistics()
{
  int comma_index=18;
  while(RS485_last_buff[comma_index]!=',' && comma_index<23)
    comma_index++;
  char tarray[15];
  RS485_last_buff.substring(11,comma_index).toCharArray(tarray, sizeof(tarray));
  long long temp = char2LL(tarray);
  difference=temp-last_ping_time;
  last_ping_time=temp;
}

long long DesertStar::char2LL(char *str)
{
  long long result = 0; // Initialize result
  // Iterate through all characters of input string and update result
  for (int i = 0; str[i] != '\0'; ++i)
    result = result*10 + str[i] - '0';
  return result;
}

void DesertStar::print(String output)
{
  if(debug)
    Serial.print(output);
  if(memory_card)
    memory_card->print(output);
}

void DesertStar::print(char* output)
{
  if(debug)
    Serial.print(output);
  if(memory_card)
    memory_card->print(output);
}

void DesertStar::println(String output)
{
  if(debug)
    Serial.println(output);
  if(memory_card)
    memory_card->println(output);
}

void DesertStar::println(char* output)
{
  if(debug)
    Serial.println(output);
  if(memory_card)
    memory_card->println(output);
}

void DesertStar::print_last_buffer()
{
  print(RS485_last_buff);
}

long long DesertStar::get_ping_difference()
{
  return difference;
}

long long DesertStar::get_time_of_last_ping()
{
  return last_ping_time;
} 

