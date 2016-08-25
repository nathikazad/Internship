/*
  zI2C.h - TWI/I2C library for Arduino
  Based om twi.h by Nicholas Zambetti

  I changed the TWI (Wire & twi) library so that it does not use memory buffers for receiving and sending data.
  Increased speed of read / write operations,
  because i stopped using the copy r/w buffer before sending or after reading the data.


*/

#ifndef ZI2C_H
#define ZI2C_H

#if defined(ARDUINO)
    #include <pins_arduino.h>
    #include <Arduino.h>
    #include <util/twi.h>
#else
    #include <inttypes.h>
    #include <math.h>
    #include <stdlib.h>
    #include <avr/io.h>
    #include <avr/interrupt.h>
    #include <compat/twi.h>
    #include <util/delay.h>
    #include <stdio.h>
    #include <uart0.h>

    // NOTE: Arduino Mega pins
    #define SDA    1
    #define SCL    2
#endif
//#include <recoonKeypad.h>


/*
#ifndef TWI_FREQ
#define TWI_FREQ 100000L
#endif
*/

/*
#ifndef TWI_BUFFER_LENGTH
#define TWI_BUFFER_LENGTH 32
#endif
*/

#define TWI_READY 0
#define TWI_MRX   1
#define TWI_MTX   2
#define TWI_SRX   3
#define TWI_STX   4

/*#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif*/

  struct TtwiSlave {
     uint8_t    address;
     TtwiSlave *next;
  };


  void twi_init(unsigned long bus_freq);
  void twi_setAddress(uint8_t);
  uint16_t twi_readFrom(uint8_t, uint8_t*, uint16_t, uint8_t);
  uint8_t twi_writeTo(uint8_t address, uint8_t *provision, uint8_t provisionLen, uint8_t* data, uint16_t length, uint8_t wait, uint8_t sendStop);
  //uint8_t twi_writeTo(uint8_t, uint8_t*, uint8_t, uint8_t, uint8_t);
  uint8_t twi_transmit(const uint8_t*, uint8_t);
  void twi_attachSlaveRxEvent( void (*)(uint8_t*, int) );
  void twi_attachSlaveTxEvent( void (*)(void) );
  void twi_reply(uint8_t);
  void twi_stop(void);
  void twi_releaseBus(void);


  uint16_t twiWriteBlock(uint8_t deviceAddr, uint16_t memAddr, uint8_t *data, uint16_t len, uint8_t bpw, uint8_t wct);
  uint16_t twiReadBytes (uint8_t deviceAddr, uint16_t memAddr, uint8_t *data, uint16_t len);
  uint8_t  twiWriteBytes(uint8_t deviceAddr, uint16_t memAddr, uint8_t *data, uint8_t len, uint8_t wct = 0);
  void     twiDiscoverSlaves();
  uint8_t  getSlavesCount();
  uint8_t  getSlaveAddress(uint8_t order);
#endif

/*

//Exapmple



This example shows how to work with the i2c EEPROM:
1. Set i2c bus frequency
2. Discovering i2c EEPROM device address
3. Write single page
4. Read single page
5. Write large amount of data (a few pages at a time)
6. Read large amount of data
7. Checksum verification
8. Calculation of read / write speed
9. Increase the bus frequency and goto to step 1

You can set any address (by jumpers) on I2C EEPROM device. Address will be detected automatically.

Note:
EEPROM data writing speed is determined by the following parameters:
- EEPROM Page Size  (see datasheet for EEPROM_PAGE_SIZE constant)
- Write Cycle Time  (see datasheet for EEPROM_WRITE_CYCLE constant)
- TWI Bus frequency (TWBR register value)
- the Algorithmic delay

EEPROM data reading speed is determined by the:
- TWI Bus frequency and
- the Algorithmic delay

I changed the TWI (Wire & twi) library so that it does not use internal buffers for receiving and sending data.
So speed of read / write operations increased,
because we stopped using the copying r/w buffer before sending or after reading the data.

Have a nice rw!



#include <zI2C.h>

// The write cycle time tWR
// is the time from a valid stop condition of a write sequence
// to the end of the internal clear/write cycle (EEPROM datasheet)
#define EEPROM_WRITE_CYCLE 5  // See EEPROM datasheet

// The device also has a page write capability of up to 64 bytes of data
#define EEPROM_PAGE_SIZE   64  // See EEPROM datasheet


#define MINIMUM_BUS_FREQUENCY 400000L
unsigned long bus_frequency = MINIMUM_BUS_FREQUENCY; // Hz
                                                     // Note! Bus freq must be less than 1/16 CPUFREQ
                                                     // and less than maximum enabled freq for connected i2c device (see datasheet)
                                                     // For breadboard schematics the bus parameters are deteriorate.
                                                     // Therefore, the real speed of the bus for its stable operation
                                                     // should be less than the maximum allowable.

#define BUS_FREQUENCY_STEP 10000L                    // for scientific purposes :)


uint8_t eepromAddress;

#define LARGE_DATA_SIZE  1920

#define LOG_ENABLED 0                                // set to 1 if you want to get a detailed report


void setup() {
  randomSeed(analogRead(5));
  Serial.begin(115200);
  delay(500);

}

void loop() {
  twi_init(bus_frequency);
  Serial.println();
  Serial.print("Try the i2c bus at ");Serial.print(bus_frequency, DEC);Serial.println(" Hz...");

  if (LOG_ENABLED) Serial.println("-------------DISCOVER SLAVES---------------");
  twiDiscoverSlaves();
  // if no i2c slave devices found, getSlavesCount() returns ZERO
  if (getSlavesCount() == 0) {
    Serial.println("No i2c slave device found!");
    return;
  }
  if (LOG_ENABLED) {
    Serial.print("Slaves found : ");Serial.println(getSlavesCount(), DEC);
  }
  // assumption is that only a single i2c EEPROM is connected to the Arduino
  eepromAddress = getSlaveAddress(0); // 0 - first device, 1 - second etc....
  if (LOG_ENABLED) Serial.println();
  if (LOG_ENABLED) Serial.println("-------------SINGLE PAGE WRITE DATA---------------");
  uint8_t wCRC = 0;
  // Create temporary bufer for write
  uint8_t *writeBuf = (uint8_t *) malloc(EEPROM_PAGE_SIZE);
  if (writeBuf == NULL) {
    Serial.println("single page mode: writeBuf allocation error");
    return;
  }
  // Fill bufer with random data
  uint8_t col = 0;
  for (uint16_t i = 0; i < EEPROM_PAGE_SIZE; i++) {
    uint8_t b = (uint8_t) random(255);
    wCRC ^= b;
    writeBuf[i] = b;
    if (LOG_ENABLED) {
      Serial.print("0x");Serial.print(b, HEX);Serial.print(char(9));
      col++;
      if (col == 16) {
        Serial.println();
        col = 0;
      }
    }
  }
  unsigned long ts = micros();
  uint16_t rr = twiWriteBytes(eepromAddress, 0, writeBuf, EEPROM_PAGE_SIZE, EEPROM_WRITE_CYCLE);
  ts = micros() - ts;
  if (LOG_ENABLED) Serial.println();
  if (rr) {
    if (LOG_ENABLED) {
      Serial.print("Write time             = ");  Serial.println(ts, DEC);
      Serial.print("Write operation result = ");  Serial.println(rr, DEC);
      Serial.print("write bufer CRC        = 0x");Serial.println(wCRC, HEX);
    }
  } else {
    Serial.println("single page mode: write error");
  }
  free(writeBuf);


  if (LOG_ENABLED) Serial.println();
  if (LOG_ENABLED) Serial.println("-------------SINGLE PAGE READ DATA---------------");
  uint8_t rCRC = 0;
  uint8_t *readBuf = (uint8_t *) malloc(EEPROM_PAGE_SIZE);
  if (readBuf == NULL) {
    Serial.println("single page mode: readBuf allocation error");
    return;
  }

  ts = micros();
  rr = twiReadBytes(eepromAddress, 0, readBuf, EEPROM_PAGE_SIZE);
  ts = micros() - ts;
  if (rr == 0) {
    Serial.println("single page mode: read error");
  } else {
    for (uint8_t i = 0; i < EEPROM_PAGE_SIZE; i++) {
      rCRC ^= readBuf[i];
      if (LOG_ENABLED) {
        Serial.print("0x");Serial.print(readBuf[i], HEX);Serial.print(char(9));
        col++;
        if (col == 16) {
          Serial.println();
          col = 0;
        }
      }
    }
    if (LOG_ENABLED) {
      Serial.println();
      Serial.print("Read time             = ");  Serial.println(ts, DEC);
      Serial.print("Read operation result = ");  Serial.println(rr, DEC);
      Serial.print("read bufer CRC        = 0x");Serial.println(rCRC, HEX);
    }
  }
  free(readBuf);

  if (LOG_ENABLED) Serial.println();
  if (LOG_ENABLED) Serial.println("-------------SINGLE PAGE R/W VERIVICATION---------------");

  if (wCRC == rCRC) {
    if (LOG_ENABLED) Serial.println("No error");
  } else {
    Serial.println("single page mode: wrong CRC!");
  }


  if (LOG_ENABLED) {
    Serial.println();
    Serial.println("-------------WRITING LARGE AMOUNT OF DATA---------------");
  }
  uint16_t writeSpeed = 0;
  uint16_t readSpeed = 0;


  writeBuf = (uint8_t *) malloc(LARGE_DATA_SIZE);
  if (writeBuf == NULL) {
    Serial.println("large amount of data mode: writeBuf allocation error");
    return;
  }
  // Fill bufer with random data
  wCRC = 0;
  for (uint16_t i = 0; i < LARGE_DATA_SIZE; i++) {
    uint8_t b = (uint8_t) random(255);
    wCRC ^= b;
    writeBuf[i] = b;
  }
  ts = micros();
  rr = twiWriteBlock(eepromAddress, 0, writeBuf, LARGE_DATA_SIZE, EEPROM_PAGE_SIZE, EEPROM_WRITE_CYCLE);
  ts = micros() - ts;
  if (rr) {
    if (LOG_ENABLED) {
      Serial.print("Write time             = ");  Serial.println(ts, DEC);
      Serial.print("Write operation result = ");  Serial.println(rr, DEC);
      Serial.print("write bufer CRC        = 0x");Serial.println(wCRC, HEX);
    }
  } else {
    Serial.println("large amount of data mode: write error");
  }
  if (LOG_ENABLED) Serial.println();
  free(writeBuf);
  writeSpeed = LARGE_DATA_SIZE * 1000000 / ts;


  if (LOG_ENABLED) Serial.println("-------------READING LARGE AMOUNT OF DATA---------------");
  rCRC = 0;
  readBuf = (uint8_t *) malloc(LARGE_DATA_SIZE);
  if (readBuf == NULL) {
    Serial.println("large amount of data mode: readBuf allocation error");
    return;
  }

  ts = micros();
  rr = twiReadBytes(eepromAddress, 0, readBuf, LARGE_DATA_SIZE);
  ts = micros() - ts;
  if (rr == 0) {
    Serial.println("large amount of data mode: read error");
  } else {
    for (uint16_t i = 0; i < LARGE_DATA_SIZE; i++) {
      rCRC ^= readBuf[i];
    }
    if (LOG_ENABLED) {
      Serial.print("Read time             = ");  Serial.println(ts, DEC);
      Serial.print("Read operation result = ");  Serial.println(rr, DEC);
      Serial.print("read bufer CRC        = 0x");Serial.println(rCRC, HEX);
      Serial.println();
    }
    readSpeed = LARGE_DATA_SIZE * 1000000 / ts;
  }
  free(readBuf);

  if (LOG_ENABLED) Serial.println();
  if (LOG_ENABLED) Serial.println("-------------LARGE AMOUNT OF DATA R/W VERIVICATION---------------");
  if (wCRC == rCRC) {
    Serial.println();
    Serial.println("-------------FINAL REPORT---------------");
    Serial.println("The device is working properly (no error)");
    Serial.print("TWI frequency          = ");  Serial.print(bus_frequency, DEC); Serial.println(" Hz");
    Serial.print("Write speed            = ");  Serial.print(writeSpeed, DEC);    Serial.println(" bytes/sec");
    Serial.print("Read speed             = ");  Serial.print(readSpeed, DEC);     Serial.println(" bytes/sec");
    bus_frequency += BUS_FREQUENCY_STEP;
  } else {
    Serial.println("large amount of data mode: wrong CRC!");
    bus_frequency = MINIMUM_BUS_FREQUENCY;
  }

  if (LOG_ENABLED) { // check for memory leaks
    Serial.println();
    Serial.println();
    Serial.print("Free mem : ");
    Serial.println(freeMemory(), DEC);
  }

  delay(1000);
}


extern unsigned int __heap_start;
extern void *__brkval;


//The free list structure as maintained by the
// avr-libc memory allocation routines.
//
struct __freelist {
  size_t sz;
  struct __freelist *nx;
};

// The head of the free list structure
extern struct __freelist *__flp;

// Calculates the size of the free list
int freeListSize() {
  struct __freelist* current;
  int total = 0;

  for (current = __flp; current; current = current->nx) {
    total += 2; // Add two bytes for the memory block's header
    total += (int) current->sz;
  }

  return total;
}

int freeMemory() {
  int free_memory;

  if ((int)__brkval == 0) {
    free_memory = ((int)&free_memory) - ((int)&__heap_start);
  } else {
    free_memory = ((int)&free_memory) - ((int)__brkval);
    free_memory += freeListSize();
  }
  return free_memory;
}

*/


