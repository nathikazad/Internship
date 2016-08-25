#include <internal_rtc_clock.h>
#include <external_rtc_clock.h>
#include <DateNTime.h>
#include <SPI.h>
#include <Wire.h>
#include <SparkFun_MS5803_I2C.h>
#include <DueFlashStorage.h>
#include <SD.h>
#include <SDdriver.h>
#include <IridiumSBD.h>
#include <DesertStar.h>
#include "submersible.h"

//Parameters
DateNTime sink_time(12,9,15,17,10,00); //DD,MM,YY,HH,MM,SS
DateNTime rise_time(12,9,15,17,13,00); //DD,MM,YY,HH,MM,SS
#define measuring_depth 1000 //millimeters
#define DEBUG true


//Objects
MS5803 sensor(ADDRESS_HIGH);
External_RTC_clock external_rtc_clock(RTC_CS);
Internal_RTC_clock internal_rtc_clock(XTAL);
DueFlashStorage dueFlashStorage;
SDdriver memory_card(MEM_CARD_CS, DEBUG);
IridiumSBD iridium(*IridiumSerial);
DesertStar dsm_pinger(DSM_ADDRESS, RS485Serial, false, RS485TxControl, &memory_card);

int motor_position=0;
int state=0;
double pressure_baseline;
char gps_buffer[200];
DateNTime transmit_gps_alarm(0);
int last_time_memory_saved_epoch_time=0;
void setup() {
  
  set_pin_directions_and_default_pin_states();
  Serial.begin(9600);
  GPSSerial->begin(9600);
  RS485Serial->begin(115200);
  IridiumSerial->begin(19200);
  iridium.attachConsole(Serial);
  Wire.begin();
  external_rtc_clock.init();
  internal_rtc_clock.init();
  internal_rtc_clock.set_dateNtime(external_rtc_clock.get_dateNtime());   
  dsm_pinger.set_as_pinger(DSM_EXT_SYNC);
  memory_card.initialize();
  memory_card.create_file();


  memory_card.print("Current Time: ");
  internal_rtc_clock.get_dateNtime().print_date_time();
  memory_card.print("Sink Time: ");
  sink_time.print_date_time();
  memory_card.print("Rise Time: ");
  rise_time.print_date_time();  
  get_state_from_flash();
  get_motor_position_from_flash();
  memory_card.flush();
}

void loop() {
  //check for emergency conditions pressure abnormal or smtn then start rise sequence
  if(state==CHILLIN)
  {
    if(internal_rtc_clock.get_dateNtime()>sink_time)
    {
      if(internal_rtc_clock.get_dateNtime()<rise_time)
      {
        
        get_gps_and_send_to_iridium();
        memory_card.println("Sending GPS");
        set_state_to_flash(SINKIN);
        memory_card.println("Sinking");
      }
      else
      {
        set_state_to_flash(SOSIN);
        memory_card.println("SOSing");
      }
    }
    else
    {
      delay(1000); //should be go to sleep for 't' seconds
      internal_rtc_clock.get_dateNtime().print_date_time();
    }    
  }
  else if(state==SINKIN) // sinking
  {
    if(motor_position<10000) // should be (if depth*1000 < measuring_depth)
    {
      StepForwardDefault();
      digitalWrite(LED,HIGH);
      delay(900);
      digitalWrite(LED,LOW);
      delay(100);
      memory_card.print("Motor Position: ");
      memory_card.println_int(motor_position);
    }
    else
    {
      set_state_to_flash(WORKIN);
      memory_card.println("Pinging");
    }
  }
  else if(state==WORKIN) //Underwater Pinging to drifters
  {
    if(internal_rtc_clock.get_dateNtime()>rise_time)
    {
      set_state_to_flash(FIRIN);
      memory_card.println("Firing Coil Gun");
    }
    else
    {
      delay(1000);
      internal_rtc_clock.get_dateNtime().print_date_time();
    }
  }
  else if(state==FIRIN) //triggering coil gun
  {
    digitalWrite(CG_charge,HIGH);
    delay(charge_time);
    digitalWrite(CG_charge,LOW);
  
    digitalWrite(CG_position,LOW);
    delay(position_time);
    digitalWrite(CG_position,HIGH);

    delay(delay_between_position_and_fire);

    digitalWrite(CG_fire,LOW);
    delay(1000);
    digitalWrite(CG_fire,HIGH); 

    set_state_to_flash(RISIN);
    memory_card.println("Moving stepper to starting position");
  }
  else if(state==RISIN) //rising
  {
    if(motor_position>0)
    {
      StepReverseDefault();
      digitalWrite(LED,HIGH);
      delay(900);
      digitalWrite(LED,LOW);
      delay(100);
      memory_card.print("Motor Position: ");
      memory_card.println_int(motor_position);
    }
    else
    {
      set_state_to_flash(SOSIN);
      memory_card.println("Transmitting GPS data via iridium");
    }
  }
  else if(state==SOSIN) //SOSing
  {
      if(internal_rtc_clock.get_dateNtime()>transmit_gps_alarm)
      {
         memory_card.println("Sending GPS");
        get_gps_and_send_to_iridium();
        transmit_gps_alarm=DateNTime(internal_rtc_clock.get_dateNtime().get_epoch()+GPS_TRANSMISSION_PERIOD*60);
      } 
      //else if picked up switch to enjoying
  }
  else if(state==ENJOYIN) //Enjoying Life
  {
      //
  }
  unsigned long current_epoch_time=internal_rtc_clock.get_dateNtime().get_epoch();
  if(current_epoch_time-last_time_memory_saved_epoch_time>10)
  {
    memory_card.flush();
    last_time_memory_saved_epoch_time=current_epoch_time;
  }
}




