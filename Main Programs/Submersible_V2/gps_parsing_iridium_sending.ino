//These methods help with parsing the gps string and sending it via iridium, iridium settings are at default.

#include <PString.h>
#define HANDHELD_ID             "RB0008915B"

#define DRIFTER_ID_PREF "SD"
char drifterIdChar = '2';
uint8_t hour, minute, seconds, year, month, day;
uint16_t milliseconds;
// Floating point latitude and longitude value in degrees.
float latitude, longitude;
// Fixed point latitude and longitude value with degrees stored in units of 1/100000 degrees,
// and minutes stored in units of 1/100000 degrees.  See pull #13 for more details:
//   https://github.com/adafruit/Adafruit-GPS-Library/pull/13
int32_t latitude_fixed, longitude_fixed;
float latitudeDegrees, longitudeDegrees;
float geoidheight, altitude;
float speed, angle, magvariation, HDOP;
char lat, lon, mag;
boolean fix;
uint8_t fixquality, satellites;

boolean parser()
{
  char* nmea=gps_buffer;
  int32_t degree;
  long minutes;
  char degreebuff[10];
  char *p = nmea;

  // get time
  p = strchr(p, ',')+1;
  float timef = atof(p);
  uint32_t time = timef;
  hour = time / 10000;
  minute = (time % 10000) / 100;
  seconds = (time % 100);

  milliseconds = fmod(timef, 1.0) * 1000;

  p = strchr(p, ',')+1;
  // Serial.println(p);
  if (p[0] == 'A') 
    fix = true;
  else if (p[0] == 'V')
    fix = false;
  else
    return false;

  // parse out latitude
  p = strchr(p, ',')+1;
  if (',' != *p)
  {
    strncpy(degreebuff, p, 2);
    p += 2;
    degreebuff[2] = '\0';
    long degree = atol(degreebuff) * 10000000;
    strncpy(degreebuff, p, 2); // minutes
    p += 3; // skip decimal point
    strncpy(degreebuff + 2, p, 4);
    degreebuff[6] = '\0';
    long minutes = 50 * atol(degreebuff) / 3;
    latitude_fixed = degree + minutes;
    latitude = degree / 100000 + minutes * 0.000006F;
    latitudeDegrees = (latitude-100*int(latitude/100))/60.0;
    latitudeDegrees += int(latitude/100);
  }
  
  p = strchr(p, ',')+1;
  if (',' != *p)
  {
    if (p[0] == 'S') latitudeDegrees *= -1.0;
    if (p[0] == 'N') lat = 'N';
    else if (p[0] == 'S') lat = 'S';
    else if (p[0] == ',') lat = 0;
    else return false;
  }
  
  // parse out longitude
  p = strchr(p, ',')+1;
  if (',' != *p)
  {
    strncpy(degreebuff, p, 3);
    p += 3;
    degreebuff[3] = '\0';
    degree = atol(degreebuff) * 10000000;
    strncpy(degreebuff, p, 2); // minutes
    p += 3; // skip decimal point
    strncpy(degreebuff + 2, p, 4);
    degreebuff[6] = '\0';
    minutes = 50 * atol(degreebuff) / 3;
    longitude_fixed = degree + minutes;
    longitude = degree / 100000 + minutes * 0.000006F;
    longitudeDegrees = (longitude-100*int(longitude/100))/60.0;
    longitudeDegrees += int(longitude/100);
  }
  
  p = strchr(p, ',')+1;
  if (',' != *p)
  {
    if (p[0] == 'W') longitudeDegrees *= -1.0;
    if (p[0] == 'W') lon = 'W';
    else if (p[0] == 'E') lon = 'E';
    else if (p[0] == ',') lon = 0;
    else return false;
  }
  // speed
  p = strchr(p, ',')+1;
  if (',' != *p)
  {
    speed = atof(p);
  }
  
  // angle
  p = strchr(p, ',')+1;
  if (',' != *p)
  {
    angle = atof(p);
  }
  
  p = strchr(p, ',')+1;
  if (',' != *p)
  {
    uint32_t fulldate = atof(p);
    day = fulldate / 10000;
    month = (fulldate % 10000) / 100;
    year = (fulldate % 100);
  }
  // we dont parse the remaining, yet!
  return true;
}

int format_gps_string_and_send_to_iridium()
{      
  parser();
  char retrieveMessage[ 50 ]; 
  PString retMessageStr( retrieveMessage, sizeof(retrieveMessage));
  retMessageStr.begin();
  retMessageStr.print(HANDHELD_ID);
  retMessageStr.print(drifterIdChar);
  retMessageStr.print(" ");
  retMessageStr.print(getDegree(latitude));
  retMessageStr.print(" ");
  retMessageStr.print(getMin(latitude),2);
  retMessageStr.print(lat);
  retMessageStr.print(" ");
  retMessageStr.print(getDegree(longitude));
  retMessageStr.print(" ");
  retMessageStr.print(getMin(longitude),2);
  retMessageStr.print(lon);
  retMessageStr.print(" ");
  retMessageStr.print(hour);
  retMessageStr.print(":");          
  retMessageStr.println(minute); 
  memory_card.print("Sending String: ");
  memory_card.println(retrieveMessage);
  return iridium.sendSBDText(retrieveMessage); 
//  
}

float getMin( float degMin ){
   return fmod( (double)degMin, 100.0);
  }

int getDegree( float degMin ){
  return (int) (degMin / 100);
}
void get_gps_and_send_to_iridium()
{
  while(GPSSerial->available()) //Get Rid of OLD BUFFER DATA
    GPSSerial->read();
  while(!GPSSerial->available())
    ;
  int gps_read_char_index=0;
  char gps_read_char;
  while(gps_read_char!='\n') 
  {
    if(GPSSerial->available())
    {
      gps_read_char=GPSSerial->read();
      Serial.print(gps_read_char);
      gps_buffer[gps_read_char_index]=gps_read_char;
      gps_read_char_index++;
    }
  }
  gps_buffer[gps_read_char_index]=0;
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

  memory_card.print("Signal quality is ");
  memory_card.println(signalQuality); 

  err = format_gps_string_and_send_to_iridium();
  if (err != 0)
  {
    memory_card.print("sendSBDText failed: error ");
    memory_card.println(err);
    return;
  }
  memory_card.println("GPS Co-ordinates Sent");
  memory_card.println();
}

bool ISBDCallback()
{
  //emergency_procedures
  return true;
}

