/*
  Program modes
*/
#define Authenticate 1 // Used if there are multiple authentication account available
#define _DEBUGMAIN_ 0 // For printing stuff on the serial monitor. 
                      // Dont forget to also activate #define DEBUG_TA in "Struct.h" file for debugging

#include "ta232401006.h"

/****** ESP32 HIBERNATE DURATION AND TRIGGER INITIALIZATION ********/
uint64_t SLEEP_DURATION = (uint64_t)86400 * (uint64_t)1000000; // this is in microseconds
uint64_t sleepBitMask = pow(2,13); // Only for force pin, tamper pin is dynamicly configured

//ABP Credentials
/*
  Notes:
  - select ABP Activation on ANTARES
  - select inherit to generate your keys
  - nwkSKey: 32 digit hex, you can put 16 first digits by first 16 digits your access key and add 16 digits with 0 (ex : abcdef01234567890000000000000000)
  - appSKey: 32 digit hex, put 16 first digits by 0 and put last 16 digits by last 16 digit your access key (ex : 0000000000000000abcdef0123456789)
*/

#if Authenticate == 1
const char *devAddr = "xxxxxxxx"; // Replace with the Device Address that you have in the Antares console
const char *nwkSKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; // Replace with the Network Session Key that you have in the Antares console
const char *appSKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; // Replace with the Application Session Key that you have in the Antares console

#elif Authenticate == 2
const char *devAddr = "xxxxxxxx"; // Replace with the Device Address that you have in the Antares console
const char *nwkSKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; // Replace with the Network Session Key that you have in the Antares console
const char *appSKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; // Replace with the Application Session Key that you have in the Antares console

#elif Authenticate == 3 
const char *devAddr = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char *nwkSKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char *appSKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

#elif Authenticate == 4 
const char *devAddr = "xxxxxxxx";
const char *nwkSKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char *appSKey = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
#endif

const sRFM_pins RFM_pins = {
    .CS = 5,    // LYNX32 to RFM NSS
    .RST = 14,  // LYNX32 to RFM RST
    .DIO0 = 2,  // LYNX32 to RFM DIO0
    .DIO1 = 27, // LYNX32 to RFM DIO1
};

#ifdef _SERIALPORT2_
HardwareSerial SerialPort(2); // Using UART2
#endif

void setup() {
  // Setup loraid access and serialport init for MSP and PC
  #if _DEBUGMAIN_
  Serial.begin(115200); // For PC comm
  #endif
  SerialPort.begin(9600, SERIAL_8N1, 16, 17); // For MSP comm
  delay(200);
  #if _DEBUGMAIN_
  Serial.println("Begin Program...");
  #endif

  // Modify nomor kecamatan and nomor pelanggan. Default is 1
  noKec = 1;
  noPel = 1;
  setModemSleep();

  // Initialize EEPROM and read address 0 for tamperflag
  EEPROM.begin(1);
  flagTamper_ = EEPROM.read(0);
  #if _DEBUGMAIN_
  Serial.printf("flagTamper: %d\n",flagTamper_);
  #endif

  // Main Program
  print_wakeup_reason(SLEEP_DURATION, sleepBitMask);  

  // Put ESP to sleep zzzZZZ
  hibernate(sleepBitMask);
}

void loop() {}
