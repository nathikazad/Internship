#include <DesertStar.h>
#include <SPI.h>
#include <SD.h>
#include <SDdriver.h>
#include <Wire.h>
#include <IridiumSBD.h>

#define DEBUG true
#define MEM_CARD_CS 8
#define GPS_PPS_PIN 9
#define RS485TxControl 10  
#define DSM_ADDRESS "3980"
#define DSM_THRESHOLD "120"
#define DSM_EXT_SYNC true

#define iridium_on true
#define iridium_update_period 3000 //in seconds


HardwareSerial *RS485Serial=&Serial3;
HardwareSerial *IridiumSerial=&Serial2;

SDdriver memory_card(MEM_CARD_CS, DEBUG);
DesertStar dsm_receiver(DSM_ADDRESS, RS485Serial, false, RS485TxControl, &memory_card);
IridiumSBD iridium(*IridiumSerial);

char gps_buffer[200];
boolean gps_received=false;

boolean iridium_initialized=false;
boolean counter_reset=false;
int32_t seconds_elapsed=-4;

void setup() {
  
  Serial.begin(9600);
  Wire.begin();  
  RS485Serial->begin(115200);
  Serial2.begin(19200);
  memory_card.initialize();
  memory_card.create_file();
  if(iridium_on)
    init_iridium();
  dsm_receiver.set_as_receiver(DSM_THRESHOLD, DSM_EXT_SYNC);
  attachInterrupt(GPS_PPS_PIN, new_gps_pulse_received, RISING);
  memory_card.println("Surface Drifter V1");
  memory_card.flush();

}

void loop() {
  looped_procedures();
  if(iridium_on)
    send_gps_co_ords_if_it_is_time_for_it();
}

bool ISBDCallback()
{
  if(iridium_initialized)
  {
    looped_procedures();
  }
  return true;
}

void looped_procedures()
{  
  if(gps_received)
  {
    seconds_elapsed++;
    print_gps();
    counter_reset_if_it_already_hasnt();
    dsm_receiver.ask_last_ping();
  }
  print_ping_info_if_last_ping_info_received();
}

void new_gps_pulse_received()
{
  gps_received=true;
}

void init_iridium()
{
  iridium.attachConsole(Serial);
  int signalQuality = -1;
  int err=iridium.begin();
  if (err != 0)
  {
    memory_card.print("Iridium Begin failed: error ");
    memory_card.println(err);
    return;
  }

  err = iridium.getSignalQuality(signalQuality);
  if (err != 0)
  {
    memory_card.print("SignalQuality failed: error ");
    memory_card.println(err);
    return;
  }

  Serial.print("Signal quality is ");
  Serial.println(signalQuality); 

  err = iridium.sendSBDText("Hello from Receiver");
  if (err != 0)
  {
    memory_card.print("sendSBDText failed: error ");
    memory_card.println(err);
    return;
  }
  memory_card.println("Message sent to iridium!");
  memory_card.println();
  iridium_initialized=true;
}

void print_gps()
{
  int index=0;
  while(true)
  {
    Wire.requestFrom(0x42, 1);    // request 6 bytes from slave device #2
    char c = Wire.read(); // receive a byte as character
    if(c<255)
    {
      gps_buffer[index]=c;
      index++;
    }
    else
      break;
  }
  gps_buffer[index]=0;
  memory_card.print(gps_buffer);
  memory_card.flush();
  gps_received=false;
}

void print_ping_info_if_last_ping_info_received()
{
  if(dsm_receiver.last_ping_received())
  {
    dsm_receiver.print_last_buffer();
    memory_card.println();
    memory_card.println();
    memory_card.flush();
  }
}
void send_gps_co_ords_if_it_is_time_for_it()
{
  if(iridium_on && seconds_elapsed % iridium_update_period==0)
  {
    memory_card.println();
    memory_card.println("Initiating GPS Co-ordinates sending");
    int err = format_gps_string_and_send_to_iridium();
    if (err != 0)
    {
      memory_card.print("sendSBDText failed: error ");
      memory_card.println(err);
      return;
    }
     memory_card.println("GPS Co-ordinates Sent");
      memory_card.println();
  }
  //Add Fallback in case of GPS pulse generation failure.
}
void counter_reset_if_it_already_hasnt()
{
  if(!counter_reset && seconds_elapsed>0)
  {
    uint32_t pinger_reset_start_time=millis();
    dsm_receiver.reset_sync_counter();
    memory_card.print("PINGER RESET!");//, took a total time of ");
    memory_card.println();
    counter_reset=true;
  }
}
