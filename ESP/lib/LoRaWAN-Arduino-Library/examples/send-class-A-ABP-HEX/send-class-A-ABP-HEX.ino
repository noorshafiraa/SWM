/**
   Example of ABP device
   Authors:
          Ivan Moreno
          Eduardo Contreras
    June 2019

   This code is beerware; if you see me (or any other collaborator
   member) at the local, and you've found our code helpful,
   please buy us a round!
   Distributed as-is; no warranty is given.
*/
#include <lorawan.h>

//ABP Credentials
const char *devAddr = "Lora-Device-Address";
const char *nwkSKey = "Network-Session-Key";
const char *appSKey = "Application-Session-Key";


const unsigned long interval = 10000;    // 10 s interval to send message
unsigned long previousMillis = 0;  // will store last time message sent
unsigned int counter = 0;     // message counter

char myStr[50];
byte outStr[255];
byte recvStatus = 0;
int port, channel, freq;
bool newmessage = false;

const sRFM_pins RFM_pins = {
  .CS = 5,
  .RST = 0,
  .DIO0 = 27,
  .DIO1 = 2,
};


void setup() {
  // Setup loraid access
  Serial.begin(115200);
  delay(2000);
  if (!lora.init()) {
    Serial.println("RFM95 not detected");
    delay(5000);
    return;
  }

  // Set LoRaWAN Class change CLASS_A or CLASS_C
  lora.setDeviceClass(CLASS_A);

  // Set Data Rate
  lora.setDataRate(SF10BW125);

  // Set FramePort Tx
  lora.setFramePortTx(5);

  // set channel to random
  lora.setChannel(MULTI);

  // Set TxPower to 15 dBi (max)
  lora.setTxPower(15);

  // Put ABP Key and DevAddress here
  lora.setNwkSKey(nwkSKey);
  lora.setAppSKey(appSKey);
  lora.setDevAddr(devAddr);
}

void loop() {
  // Check interval overflow
  if (millis() - previousMillis > interval) {
    previousMillis = millis();


    byte myByte[] = {0xFF, 0xA2, 0x33, 0xD4, 0x55};
    Serial.print("Sending: ");
    for (int i = 0; i < sizeof(myByte); i++)
    {
      Serial.print(myByte[i]); Serial.print(" ");
    }
    Serial.println();


    lora.sendUplinkHex(myByte, sizeof(myByte), 0);
    port = lora.getFramePortTx();
    channel = lora.getChannel();
    freq = lora.getChannelFreq(channel);
    Serial.print(F("fport: "));    Serial.print(port); Serial.print(" ");
    Serial.print(F("Ch: "));    Serial.print(channel); Serial.print(" ");
    Serial.print(F("Freq: "));    Serial.print(freq); Serial.println(" ");

  }

  // Check Lora RX
  lora.update();

  recvStatus = lora.readDataByte(outStr);
  if (recvStatus) {
    newmessage = true;
    int counter = 0;
    port = lora.getFramePortRx();
    channel = lora.getChannelRx();
    freq = lora.getChannelRxFreq(channel);

    for (int i = 0; i < recvStatus; i++)
    {
      if (((outStr[i] >= 32) && (outStr[i] <= 126)) || (outStr[i] == 10) || (outStr[i] == 13))
        counter++;
    }
    if (port != 0)
    {
      if (counter == recvStatus)
      {
        Serial.print(F("Received String : "));
        for (int i = 0; i < recvStatus; i++)
        {
          Serial.print(char(outStr[i]));
        }
      }
      else
      {
        Serial.print(F("Received Hex : "));
        for (int i = 0; i < recvStatus; i++)
        {
          Serial.print(outStr[i], HEX); Serial.print(" ");
        }
      }
      Serial.println();
      Serial.print(F("fport: "));    Serial.print(port); Serial.print(" ");
      Serial.print(F("Ch: "));    Serial.print(channel); Serial.print(" ");
      Serial.print(F("Freq: "));    Serial.println(freq); Serial.println(" ");
    }
    else
    {
      Serial.print(F("Received Mac Cmd : "));
      for (int i = 0; i < recvStatus; i++)
      {
        Serial.print(outStr[i], HEX); Serial.print(" ");
      }
      Serial.println();
      Serial.print(F("fport: "));    Serial.print(port); Serial.print(" ");
      Serial.print(F("Ch: "));    Serial.print(channel); Serial.print(" ");
      Serial.print(F("Freq: "));    Serial.println(freq); Serial.println(" ");
    }
  }

}
