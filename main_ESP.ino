#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
#define LED 12
#define Button 13
Adafruit_SSD1306 display(OLED_RESET);

float value = 0000.000;

void setup()
{
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.setTextColor(WHITE);
  pinMode(LED, OUTPUT);
  pinMode(Button, INPUT);
  Serial.begin(9600);

  digitalWrite(LED, HIGH);
}

void loop() {

  //Kode buat dapetin value
  // value = ;

  if (digitalRead(Button) == LOW) { //ini ditekan
    Serial.println("HIGH");
    display.clearDisplay();
    display.setTextSize(1);

    String splashString="Volume konsumsi air";
    display.setCursor(64-(splashString.length()*3),0);
    display.print(splashString);

    display.setTextSize(2);
    display.setCursor(0,16);
    display.print(value);

    display.setTextSize(1);
    splashString="3";
    display.setCursor(115,16);
    display.print(splashString);

    display.setTextSize(1);
    splashString="m";
    display.setCursor(110,19);
    display.print(splashString);

    display.display();
    delay(10000);
  }

  else {
    Serial.println("LOW");
    display.clearDisplay();
    display.display();
  }
}

