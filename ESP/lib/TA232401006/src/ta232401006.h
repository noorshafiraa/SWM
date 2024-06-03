/*
 * Library for main program TA232401006 
 *
 * SMART WATER METER
 */

#include "lorawan.h"
#include <HardwareSerial.h>
#include <WiFi.h>
#include <EEPROM.h>
#include <esp_sleep.h>

// Struct for Konsumsi Force 53 bit
struct KonsumsiForce53 {
  bool forceFlag : 1;
  uint8_t nomorKecamatan : 5;
  uint16_t nomorPelanggan : 14;
  bool flagTamper : 1;
  float konsumsiAir;
};

// Struct for Konsumsi Periodic 52 bit
struct KonsumsiPeriodic52 {
  uint8_t nomorKecamatan : 5;
  uint16_t nomorPelanggan : 14;
  bool flagTamper : 1;
  float konsumsiAir;
};



/**
  For UART
*/
#define SECONDS 'a'
#define MINUTES 'A'
#define HOURS '!'

/**
  For Device Class
*/

/**
 * PIN FOR TAMPER AND FORCE BUTTON
*/
#define TAMPER_PIN 25 // GIOP21 pin connected to button //GIOP naon
#define FORCE 13 // GIOP21 pin connected to button

/************ Lora Stuff ******************/
// Lora global variables
extern char myStr[50];
extern byte outStr[255];
extern byte recvStatus;
extern int port, channel, freq;
extern bool newmessage;
extern const unsigned long interval;    // 10 s interval to send message
extern const unsigned long periodicInterval;  // 600000;    // 10 mins
extern unsigned long previousMillis;  // will store last time message sent
extern unsigned int counter;     // message sent counter
// Define nomor pelanggan and kecamatan
extern uint8_t noKec;
extern uint16_t noPel;
// Downlink data
extern uint32_t timeDownlink;
// Tamper variable
extern uint8_t flagTamper_;
// For sending packet
extern const char *devAddr;
extern const char *nwkSKey;
extern const char *appSKey;


/****** ESP32 HIBERNATE DURATION DELCARATION ********/
extern uint64_t SLEEP_DURATION; 

/****** ESP32 COMMUNICATION WITH MSP430 USING UART2********/
#define _SERIALPORT2_
extern HardwareSerial SerialPort; // Using UART2

/************ LoRa Stuff ******************/
/**
 * @brief Making Force Data payload
 */
uint64_t combineDataForce(KonsumsiForce53 dataForce);

/**
 * @brief Making Periodic Data payload
 */
uint64_t combineDataPeriodic(KonsumsiPeriodic52 dataPeriodic);

/**
 * @brief Uplink for periodic data packet
 * Flag tamper becomes 0 or false after sending packet
 * returns 0 if Rx received
 */
bool UplinkPeriodic(float konsumsi, uint8_t flagTamper__, uint8_t nk, uint16_t np);

/**
 * @brief Uplink for force data packet
 * Flag tamper becomes 0 or false after sending packet
 * returns 0 if Rx received
 */
bool UplinkForce(float konsumsi, uint8_t flagTamper__, uint8_t nk, uint16_t np);

/**
 * @brief RFM LoRa initialization
 */
bool loraStart();

/**
 * @brief RFM LoRa Configurtion
 */
void loraConfigure();
/******************************************/


/************ UART Stuff ******************/

/**
 * @brief Getting volume from MSP via UART
 */
float uartVolume();

/**
 * @brief Getting time from MSP Via UART
 * @param SECONDS to get seconds 
 * @param MINUTES to get minutes
 * @param HOURS to get hours
 */
char uartRTC(char a);

/**
 * @brief Sending clock from downlink to MSP.
 * Make sure to only do this if downlink succeed
 */
void uartRTC_update();
/******************************************/

/************ SLEEP AND WAKING UP ******************/

/**
 * @brief Set Modem to Sleep.
 * Always use this in void setup() to reduce power
 */
void setModemSleep();

/**
 * @brief Waking up Modem. We
 * will not use this
 */
void wakeModemSleep();

/**
 * @brief Set ESP32 in hibernate Mode
 * Use this at the end of the void setup function
 */
void hibernate(uint64_t sleepMask_);

/**
 * @brief Method to print the reason by which ESP32
 * has been awaken from sleep
 * This is also the main program
 * @param sleep_time Modify the sleep time of ESP32
 * @param sleepMask_ Modify the trigger pin Mask
 */
void print_wakeup_reason(uint64_t &sleep_time, uint64_t &sleepMask_);


/**
 * @brief Calculate the sleep time after time difference
 * @param timeSeconds The time differnce of the schedule and current time
 * @return sleepPeriod, in microseconds
 */
uint64_t calc_sleep_time(int timedif_sec);

/**
 * @brief Method to print the GPIO that triggered the wakeup
 */
int print_GPIO_wake_up();