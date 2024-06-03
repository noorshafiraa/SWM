#include "ta232401006.h"

/************ Lora Stuff ******************/
// Lora global variables
char myStr[50];
byte outStr[255];
byte recvStatus = 0;
int port, channel, freq;
bool newmessage = false;
const unsigned long interval = 10000;    // 10 s interval to send message
const unsigned long periodicInterval = 60000;  // 600000;    // 10 mins
unsigned long previousMillis = 0;  // will store last time message sent
unsigned int counter = 0;     // message sent counter
// Define nomor pelanggan and kecamatan
uint8_t noKec = 1;
uint16_t noPel = 1;
// Downlink data
uint32_t timeDownlink;
// Tamper variable
uint8_t flagTamper_;



/**     LOCAL FUNCTIONS     **/

uint64_t combineDataForce(KonsumsiForce53 dataForce) {
    uint64_t combinedData = 0;

    combinedData |= (uint64_t)dataForce.forceFlag << 52;
    combinedData |= (uint64_t)dataForce.nomorKecamatan << 47;
    combinedData |= (uint64_t)dataForce.nomorPelanggan << 33;
    combinedData |= (uint64_t)dataForce.flagTamper << 32;
    // Convert float to 32-bit integer for storage
    uint32_t konsumsiForceAir = *(uint32_t*)&dataForce.konsumsiAir;
    combinedData |= (uint64_t)konsumsiForceAir;

    return combinedData;
}

uint64_t combineDataPeriodic(KonsumsiPeriodic52 dataPeriodic) {
  uint64_t combinedData = 0;

  combinedData |= static_cast<uint64_t>(dataPeriodic.nomorKecamatan) << 47;
  combinedData |= static_cast<uint64_t>(dataPeriodic.nomorPelanggan) << 33;
  combinedData |= static_cast<uint64_t>(dataPeriodic.flagTamper) << 32;

  // Convert float to 32-bit integer for storage
  uint32_t konsumsiPeriodicAir;
  memcpy(&konsumsiPeriodicAir, &dataPeriodic.konsumsiAir, sizeof(konsumsiPeriodicAir));
  combinedData |= static_cast<uint64_t>(konsumsiPeriodicAir);

  return combinedData;
}

// Uplink for Periodic Data
bool UplinkPeriodic(float konsumsi, uint8_t flagTamper__, uint8_t nk, uint16_t np){
  // Make packet to sent by using combinedData function
  KonsumsiPeriodic52 dataPeriodic = {nk, np, flagTamper__, konsumsi}; //ID_Perangkat 01_00001
  uint64_t data2sent = combineDataPeriodic(dataPeriodic);

  byte dataBYTES[7];

  // Convert to big-endian
  for (int i = 0; i < 7; i++) {
    dataBYTES[6 - i] = (data2sent >> (i * 8)) & 0xFF;
  }

  // Print the byte array in hexadecimal format
  #ifdef DEBUG_TA
  Serial.print("Data in HEX: ");
  for (int i = 0; i < sizeof(dataBYTES); i++) {
      if (dataBYTES[i] < 0x10) {
          Serial.print('0'); // Add leading zero for single digit hex values
      }
      Serial.print(dataBYTES[i], HEX);
      Serial.print(' ');
  }
  Serial.println();

  channel = lora.getChannel();
  Serial.print(F("Ch : "));    
  Serial.print(channel); 
  Serial.println(" ");
  #endif

  lora.sendUplinkHex(dataBYTES, sizeof(dataBYTES), 0);
 

  // Lora Tx and Rx cycle
  lora.update();

  recvStatus = lora.readDataByte(outStr);
  if (recvStatus) 
  {
    for (int i = 0; i < recvStatus; i++)
    {
      timeDownlink = (timeDownlink << 8) | outStr[i];
    }
    #ifdef DEBUG_TA
    // Print the uint64_t value
    Serial.print(F("Converted uint32_t value: "));
    Serial.println(timeDownlink);
    #endif
    
  }
  
  else{
    #ifdef DEBUG_TA
    Serial.println("Do Tx 1 more time...");
    channel = lora.getChannel();
    Serial.print(F("Ch : "));    
    Serial.print(channel); 
    Serial.println(" ");
    #endif
    
    lora.sendUplinkHex(dataBYTES, sizeof(dataBYTES), 0);
    lora.update();

    recvStatus = lora.readDataByte(outStr);
    if (recvStatus) 
    {
      for (int i = 0; i < recvStatus; i++)
      {
        timeDownlink = (timeDownlink << 8) | outStr[i];
      }
      #ifdef DEBUG_TA
      // Print the uint64_t value
      Serial.print(F("Converted uint32_t value: "));
      Serial.println(timeDownlink);
      #endif
    }
  }

  flagTamper_ = false;
#ifdef DEBUG_TA
  Serial.println("Tampering Reset :)");
  Serial.println(flagTamper_);
#endif

  return recvStatus;
}

// Uplink for Force Data
bool UplinkForce(float konsumsi, uint8_t flagTamper__, uint8_t nk, uint16_t np)
{
  // Make packet to sent by using combinedData function
  KonsumsiForce53 dataForce = {1, nk, np, flagTamper__, konsumsi}; //ID_Perangkat 01_00001
  uint64_t data2sent = combineDataForce(dataForce);
  
  byte dataBYTES[7];

  // Convert to big-endian
  for (int i = 0; i < 7; i++) {
    dataBYTES[6 - i] = (data2sent >> (i * 8)) & 0xFF;
  }

  // Print the byte array in hexadecimal format
  #ifdef DEBUG_TA
  Serial.print("Data in HEX: ");
  for (int i = 0; i < sizeof(dataBYTES); i++) {
      if (dataBYTES[i] < 0x10) {
          Serial.print('0'); // Add leading zero for single digit hex values
      }
      Serial.print(dataBYTES[i], HEX);
      Serial.print(' ');
  }
  Serial.println();

  channel = lora.getChannel();
  Serial.print(F("Ch : "));    
  Serial.print(channel); 
  Serial.println(" ");
  #endif

  lora.sendUplinkHex(dataBYTES, sizeof(dataBYTES), 0);
 

  // Lora Tx and Rx cycle
  lora.update();

  recvStatus = lora.readDataByte(outStr);
  if (recvStatus) 
  {
    for (int i = 0; i < recvStatus; i++)
    {
      timeDownlink = (timeDownlink << 8) | outStr[i];
    }
#ifdef DEBUG_TA
    // Print the uint64_t value
    Serial.print(F("Converted uint32_t value: "));
    Serial.println(timeDownlink);
#endif
  }
  else{
#ifdef DEBUG_TA
    Serial.println("Do Tx 1 more time...");
    channel = lora.getChannel();
    Serial.print(F("Ch : "));    
    Serial.print(channel); 
    Serial.println(" ");
#endif
   
    lora.sendUplinkHex(dataBYTES, sizeof(dataBYTES), 0);
    lora.update();
    

    recvStatus = lora.readDataByte(outStr);
    if (recvStatus) 
    {
      for (int i = 0; i < recvStatus; i++)
      {
        timeDownlink = (timeDownlink << 8) | outStr[i];
      }
#ifdef DEBUG_TA
      // Print the uint64_t value
      Serial.print(F("Converted uint32_t value: "));
      Serial.println(timeDownlink);
#endif
    }
  }

  flagTamper_ = false;
#ifdef DEBUG_TA
  Serial.println("Tampering Reset :)");
  Serial.println(flagTamper_);
#endif

  return recvStatus;
}

//Function for getting water volume from MSP, return float
float uartVolume(){
  float receivedFloat;

  // Memasuki bagian megirim perintah ke MSP
  while (SerialPort.available() > 0) {
    // Memastikan bahwa data yang diterima adalah data baru
    char discard = SerialPort.read();
    #ifdef DEBUG_TA
    Serial.println("Menghapus Byte pada pada RXuart2");
    #endif
  }

  unsigned long prevTime = millis();
  SerialPort.print('5'); // Send initial data

  while (SerialPort.available() == 0) {
    unsigned long currentTime = millis();
    if (currentTime - prevTime >= 50) {
        SerialPort.print('5'); // Send data repeatedly
        prevTime = currentTime;
    }
    delay(5); // Add a small delay to prevent busy loop
  }

  // Read float value
  if (SerialPort.available() > 0) {
    uint8_t buffer[sizeof(float)];
    // SerialPort.readBytes(reinterpret_cast<uint8_t*>(&receivedFloat), sizeof(float)); di comment soalnya belom dicoba, ga tau berhasil ato engga
  
    // Read bytes into buffer
    SerialPort.readBytes(buffer, sizeof(float));
    // Copy bytes from buffer into float variable
    memcpy(&receivedFloat, buffer, sizeof(float));
  }
  return receivedFloat;
}

char uartRTC(char a)
{
  char receivedChar;
  // Memasuki bagian megirim perintah ke MSP
  while (SerialPort.available() > 0) {
    // Memastikan bahwa data yang diterima adalah data baru
    char discard = SerialPort.read();
    #ifdef DEBUG_TA
    Serial.println("Menghapus Byte pada pada RXuart2");
    #endif
  }

  unsigned long prevTime = millis();
  SerialPort.print(a); // Send initial data

  while (SerialPort.available() == 0) {
    unsigned long currentTime = millis();
    if (currentTime - prevTime >= 800) { // Normalnya 50ms nunggunya
        SerialPort.print(a); // Send data repeatedly
        prevTime = currentTime;
    }
    delay(5); // Add a small delay to prevent busy loop
  }

  // Read char value
  if (SerialPort.available() >= 0) {
    receivedChar = SerialPort.read();
  }
  return receivedChar;
}

void uartRTC_update(){
  // Calculate HH:MM:SS from timeDownlink variable
  uint8_t hour = timeDownlink / 3600;
  uint8_t min = (timeDownlink - (hour * 3600)) / 60;
  uint8_t sec = (timeDownlink - (hour * 3600)) - (timeDownlink - (min * 60));

  /** Set MSP to receive Hour value **/
  while (SerialPort.available() > 0) {
    // Memastikan bahwa data yang diterima adalah data baru
    char discard = SerialPort.read();
    #ifdef DEBUG_TA
    Serial.println("Menghapus Byte pada pada RXuart2");
    #endif
  }

  unsigned long prevTime = millis();
  SerialPort.print('0'); // Send initial data

  while (SerialPort.available() == 0) {
    unsigned long currentTime = millis();
    if (currentTime - prevTime >= 800) { // Normalnya 50ms nunggunya
        SerialPort.print('0'); // Send data repeatedly
        prevTime = currentTime;
    }
    delay(5); // Add a small delay to prevent busy loop
  }

  /** Send Hour time to MSP**/
  while (SerialPort.available() > 0) {
    // Memastikan bahwa data yang diterima adalah data baru
    char discard = SerialPort.read();
    #ifdef DEBUG_TA
    Serial.println("Menghapus Byte pada pada RXuart2");
    #endif
  }

  prevTime = millis();
  SerialPort.write(hour); // Send initial data

  while (SerialPort.available() == 0) {
    unsigned long currentTime = millis();
    if (currentTime - prevTime >= 800) { // Normalnya 50ms nunggunya
        SerialPort.write(hour); // Send data repeatedly
        prevTime = currentTime;
    }
    delay(5); // Add a small delay to prevent busy loop
  }

  /** Set MSP to receive minute value**/
  while (SerialPort.available() > 0) {
    // Memastikan bahwa data yang diterima adalah data baru
    char discard = SerialPort.read();
    #ifdef DEBUG_TA
    Serial.println("Menghapus Byte pada pada RXuart2");
    #endif
  }

  prevTime = millis();
  SerialPort.print(':'); // Send initial data

  while (SerialPort.available() == 0) {
    unsigned long currentTime = millis();
    if (currentTime - prevTime >= 800) { // Normalnya 50ms nunggunya
        SerialPort.print(':'); // Send data repeatedly
        prevTime = currentTime;
    }
    delay(5); // Add a small delay to prevent busy loop
  }

  /** Send minute time to MSP**/
  while (SerialPort.available() > 0) {
    // Memastikan bahwa data yang diterima adalah data baru
    char discard = SerialPort.read();
    #ifdef DEBUG_TA
    Serial.println("Menghapus Byte pada pada RXuart2");
    #endif
  }

  prevTime = millis();
  SerialPort.write(min); // Send initial data

  while (SerialPort.available() == 0) {
    unsigned long currentTime = millis();
    if (currentTime - prevTime >= 800) { // Normalnya 50ms nunggunya
        SerialPort.write(min); // Send data repeatedly
        prevTime = currentTime;
    }
    delay(5); // Add a small delay to prevent busy loop
  }

  /** Set MSP to receive second value**/
  while (SerialPort.available() > 0) {
    // Memastikan bahwa data yang diterima adalah data baru
    char discard = SerialPort.read();
    #ifdef DEBUG_TA
    Serial.println("Menghapus Byte pada pada RXuart2");
    #endif
  }

  prevTime = millis();
  SerialPort.print('='); // Send initial data

  while (SerialPort.available() == 0) {
    unsigned long currentTime = millis();
    if (currentTime - prevTime >= 800) { // Normalnya 50ms nunggunya
        SerialPort.print('='); // Send data repeatedly
        prevTime = currentTime;
    }
    delay(5); // Add a small delay to prevent busy loop
  }

  /** Send minute time to MSP**/
  while (SerialPort.available() > 0) {
    // Memastikan bahwa data yang diterima adalah data baru
    char discard = SerialPort.read();
    #ifdef DEBUG_TA
    Serial.println("Menghapus Byte pada pada RXuart2");
    #endif
  }

  prevTime = millis();
  SerialPort.write(sec); // Send initial data

  while (SerialPort.available() == 0) {
    unsigned long currentTime = millis();
    if (currentTime - prevTime >= 800) { // Normalnya 50ms nunggunya
        SerialPort.write(sec); // Send data repeatedly
        prevTime = currentTime;
    }
    delay(5); // Add a small delay to prevent busy loop
  }
}


// Configuring LoRa, make sure nwSkey appSkey devAddr has been delcared
void loraConfigure(){
  // Set LoRaWAN Class change CLASS_A only
  lora.setDeviceClass(CLASS_A);
  // Set Data Rate [25/05/2024]
  // The success rate of this DR is still not clear
  lora.setDataRate(SF12BW125);

  // Set FramePort Tx
  lora.setFramePortTx(5);

  // set channel to random, use random seed
  randomSeed(analogRead(GPIO_NUM_35));
  lora.setChannel(MULTI);

  // Set TxPower to 15 dBi (max)
  lora.setTxPower(15);

  // Put ABP Key and DevAddress here
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);
}

// Initialize and Configure LoRa 
bool loraStart(){
  bool statInit = lora.init();
  if (!statInit) {
    return 0;
  }

  // Configuring Lora with the class A, SF10BW125, Channel Multi, Power 15, 
  // ABP, and devAddr Key
  loraConfigure();
  return 1;
}

// INI MODEM SLEEP MODE// -----------------------------------------------------------------------------------------------------------------------------

void setModemSleep() {
    WiFi.setSleep(true);
    if (!setCpuFrequencyMhz(40)){
    }
    else{
      // Use this if 40Mhz is not supported
      setCpuFrequencyMhz(80);
      #ifdef DEBUG_TA
      Serial.println("40MHz is not a valid frequency! Using 80Mhz");
      #endif
    }
}
 
void active() { //wakeModemSleep
    setCpuFrequencyMhz(240);
}

// INI HIBERNATE MODE// --------------------------------------------------------------------------------------------------------------------------------

void lightSleep() {
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION);
    esp_light_sleep_start();
}

void deepSleep() {
    esp_sleep_enable_timer_wakeup(SLEEP_DURATION);
    esp_deep_sleep_start();
}

void hibernate(uint64_t sleepMask_) {
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,   ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL,         ESP_PD_OPTION_OFF);

    // Before going to sleep, set pin for ext1 wake up trigger
    esp_sleep_enable_ext1_wakeup(sleepMask_,ESP_EXT1_WAKEUP_ANY_HIGH);

    // Going to sleep zzzZZ
    deepSleep();
}

/*
Method to print the GPIO that triggered the wakeup
*/
int print_GPIO_wake_up(){
  uint64_t GPIO_reason = esp_sleep_get_ext1_wakeup_status();
  int GPIO_trig = 0;
  if (GPIO_reason!=0){
    #ifdef DEBUG_TA
    Serial.print("GPIO that triggered the wake up: GPIO ");
    Serial.println((log(GPIO_reason))/log(2), 0);
    #endif
    GPIO_trig = (int)(log(GPIO_reason)/log(2));
  }

  else{
    #ifdef DEBUG_TA
    Serial.println("Ga ketrigger dari GPIO");
    #endif
  }

  return GPIO_trig;
}

uint64_t calc_sleep_time (int timedif_sec){
  uint64_t sleepPeriod;
  if (timedif_sec > 0)
  {
    sleepPeriod = (uint64_t)timedif_sec * 1000000;
  }
  else
  {
    sleepPeriod = (86400 + (uint64_t)timedif_sec) * 1000000; // Change to microseconds
  }

  sleepPeriod += static_cast<uint64_t>(sleepPeriod * 0.0067);

  return sleepPeriod;
}

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(uint64_t &sleep_time, uint64_t &sleepMask_) {
    esp_sleep_wakeup_cause_t wakeup_reason;
    int timediff;

    wakeup_reason = esp_sleep_get_wakeup_cause();

    if (wakeup_reason == ESP_SLEEP_WAKEUP_EXT1){
      #ifdef DEBUG_TA
      Serial.println("Wakeup caused by external signal using RTC_CNTL");
      #endif
      int GPIO_trig = print_GPIO_wake_up();

      if (GPIO_trig == FORCE){
        /****** Force button ditekan ******/
        /** This one is for DEBUG_TAging **/
        #ifdef DEBUG_TA
        pinMode(GPIO_NUM_32,OUTPUT);
        digitalWrite(GPIO_NUM_32, HIGH);
        #endif
        

        // A. Mengecek LoRa
        #ifdef DEBUG_TA
        Serial.printf("noKec: %d\n",noKec);
        Serial.printf("noPel: %d\n",noPel);
        #endif
        if (!loraStart()) {
          #ifdef DEBUG_TA
          Serial.println("RFM95 not detected");
          #endif
          // B. Jika RFM95W tidak terdeteksi
          //    Meminta dan menghitung waktu dari MSP buat waktu tidur kembali
          uint8_t sec = uartRTC(SECONDS);
          uint8_t min = uartRTC(MINUTES);
          uint8_t hour = uartRTC(HOURS);
          int timeInSecs = sec + min * 60 + hour * 3600;

          timediff = 25200 - timeInSecs; // 25200 adalah jumlah detik di jam 7:00:00

          sleep_time = calc_sleep_time(timediff);
        }
        else
        {
          #ifdef DEBUG_TA
          Serial.println("RFM95 detected!");
          #endif
          // B. Meminta volume air dari MSP dan mengirim Forcedata
          float volume = uartVolume(); 
          recvStatus = UplinkForce(volume, flagTamper_, noKec, noPel);
          if (recvStatus){ // C. Mengatur waktu tidur sesuai dengan waktu dari downlink
            timediff = 25200 - timeDownlink; // 25200 adalah jumlah detik di jam 7:00:00
            sleep_time = calc_sleep_time(timediff);
            uartRTC_update();
          }
          else
          { 
            // D. Jika tidak menerima downlink
            //Meminta dan menghitung waktu dari MSP buat waktu tidur kembali
            uint8_t sec = uartRTC(SECONDS);
            uint8_t min = uartRTC(MINUTES);
            uint8_t hour = uartRTC(HOURS);
            int timeInSecs = sec + min * 60 + hour * 3600;
            timediff = 25200 - timeInSecs; // 25200 adalah jumlah detik di jam 7:00:00
            sleep_time = calc_sleep_time(timediff); // Hitung waktu tidur
          }
          // Add tamper pin as wake up trigger
          sleepMask_ += pow(2, 25);

          // Store tamper pin EEPROM to LOW
          EEPROM.write(0, 0);
          EEPROM.commit();          
        }
        /** This one is for debugging **/
        #ifdef DEBUG_TA
        digitalWrite(GPIO_NUM_32, LOW);  
        #endif    
      }
      else if (GPIO_trig == TAMPER_PIN){
        /****** Tamper Trigger ******/
        // doesnt add tamper pin as wake up trigger
        // Store tamper pin EEPROM to high
        EEPROM.write(0, 1);
        EEPROM.commit();

        //Meminta dan menghitung waktu dari MSP buat waktu tidur kembali
        uint8_t sec = uartRTC(SECONDS);
        uint8_t min = uartRTC(MINUTES);
        uint8_t hour = uartRTC(HOURS);
        int timeInSecs = sec + min * 60 + hour * 3600;

        timediff = 25200 - timeInSecs; // 25200 adalah jumlah detik di jam 7:00:00

        sleep_time = calc_sleep_time(timediff);
      }
      #ifdef DEBUG_TA
      Serial.print("timediff: ");
      Serial.println(timediff);
      Serial.print("TIDUR SELAMA ");
      Serial.print((sleep_time/1000000));
      Serial.println("Detik");  
      #endif
    }
    else if (wakeup_reason == ESP_SLEEP_WAKEUP_TIMER){
      #ifdef DEBUG_TA
      Serial.println("Wakeup caused by timer");
      #endif
      /****** Timer Trigger ******/
      /** This one is for debugging **/
      #ifdef DEBUG_TA
      pinMode(GPIO_NUM_32,OUTPUT);
      digitalWrite(GPIO_NUM_32, HIGH);
      
      Serial.printf("noKec: %d\n",noKec);
      Serial.printf("noPel: %d\n",noPel);
      #endif
      
      // A. Mengecek Jadwal Pengiriman
      uint8_t sec = uartRTC(SECONDS);
      uint8_t min = uartRTC(MINUTES);
      uint8_t hour = uartRTC(HOURS);
      int timeInSecs = sec + min * 60 + hour * 3600;
      timediff = 25200 - timeInSecs; // 25200 adalah jumlah detik di jam 7:00:00      

      // B. Mengecek LoRa
      if (!loraStart()) {
        #ifdef DEBUG_TA
        Serial.println("RFM95 not detected");
        #endif
        // B.1 Jika RFM95W tidak terdeteksi
        //    Meminta dan menghitung waktu dari MSP buat waktu tidur kembali
        sleep_time = calc_sleep_time(timediff);
      }
      else
      {
        #ifdef DEBUG_TA
        Serial.println("RFM95 detected!");
        #endif
        // B.2 Meminta volume air dari MSP dan mengirim periodic
        if (timediff < 5) // only send periodic if the timediff is less than 5 seconds
        {
          float volume = uartVolume(); 
          recvStatus = UplinkPeriodic(volume, flagTamper_, noKec, noPel);
        }
        if (recvStatus){ // C. Mengatur waktu tidur sesuai dengan waktu dari downlink
          timediff = 25200 - timeDownlink; // 25200 adalah jumlah detik di jam 7:00:00 
          sleep_time = (86400 + (uint64_t)timediff) * 1000000; // Change to microseconds
          sleep_time += static_cast<uint64_t>(sleep_time * 0.0067);
          uartRTC_update();
        }
        else
        {
          sleep_time = calc_sleep_time(timediff);
        }
        
        // Add tamper pin as wake up trigger
        sleepMask_ += pow(2, 25);

        // Store tamper pin EEPROM to LOW
        EEPROM.write(0, 0);
        EEPROM.commit();

        #ifdef DEBUG_TA
        Serial.printf("tamper flag reset :)\n tamper flag: %d\n",EEPROM.read(0));
        #endif          
      }
      /** This one is for debugging **/
      #ifdef DEBUG_TA
      digitalWrite(GPIO_NUM_32, LOW); 
      #endif     
    }
    else
    {
      #ifdef DEBUG_TA
      Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason);
      #endif

      // Adds tamper pin as wake up trigger
      if (!flagTamper_){
        #ifdef DEBUG_TA
        Serial.println("Membuat tamper trigger karena\nTamper off");
        #endif
        sleepMask_ += pow(2, 25);
      }
      else{
        #ifdef DEBUG_TA
        Serial.println("Tidak mebuat tamper trigger karena\nTamper sudah on");
        #endif
      }
      

      //Meminta dan menghitung waktu dari MSP buat waktu tidur kembali
      uint8_t sec = uartRTC(SECONDS);
      uint8_t min = uartRTC(MINUTES);
      uint8_t hour = uartRTC(HOURS);
      int timeInSecs = sec + min * 60 + hour * 3600;

      // Menghitung perbedaan waktu
      timediff = 25200 - timeInSecs; // 25200 adalah jumlah detik di jam 7:00:00
      sleep_time = calc_sleep_time(timediff);

      #ifdef DEBUG_TA
      Serial.print("timediff: ");
      Serial.println(timediff);
      Serial.print("TIDUR SELAMA ");
      Serial.print((sleep_time/1000000));
      Serial.println("Detik");  
      #endif
    }
}

