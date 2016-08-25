#define charge_time 30000 //milliseconds
#define position_time 50 //milliseconds
#define fire_time 50 //milliseconds
#define delay_between_position_and_fire 10 //milliseconds
#define fire_time 1000 //milliseconds
#define step_increment 1000 //steps
#define GPS_TRANSMISSION_PERIOD 1 //minutes

#define DSM_ADDRESS "4004"
#define DSM_EXT_SYNC false

#define RTC_CS 2
#define RS485TxControl 3
#define IRID_SLEEP 5
#define MEM_CARD_CS 8
#define GPS_PPS_PIN 9
  
#define CG_position 11
#define CG_charge 12
#define CG_fire 13
#define DIR 38
#define STP 40
#define EN  43
#define MS3 46
#define MS2 48
#define MS1 50
#define LED 53

//States
#define CHILLIN  0
#define SINKIN   1
#define WORKIN   2
#define FIRIN    3
#define RISIN    4
#define SOSIN    5 
#define ENJOYIN  6

String state_array[]={"CHILLIN","SINKIN","WORKIN","FIRIN","RISIN","SOSIN","ENJOYIN"};

HardwareSerial *GPSSerial=&Serial1;
HardwareSerial *IridiumSerial=&Serial2;
HardwareSerial *RS485Serial=&Serial3;
