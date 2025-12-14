#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>
#include <SoftwareSerial.h>

// --- SoftwareSerial : TX = 11, RX = 10 ---
SoftwareSerial mySerial(10, 11); // RX, TX

#define DS1621_ADDR 0x48
#define ACCESS_CONFIG 0xAC
#define START_CONVERT 0xEE
#define READ_TEMP 0xAA

Adafruit_BMP085 bmp180;

float tempDS1621, tempBMP180, pressure;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  Wire.begin();

  if (!bmp180.begin()) {
    Serial.println("ERREUR : BMP180 introuvable !");
    while (1);
  }

  configDS1621();
  Serial.println("SYSTEME PRET");
}

void configDS1621() {
  Wire.beginTransmission(DS1621_ADDR);
  Wire.write(ACCESS_CONFIG);
  Wire.write(0x02);  // mode one-shot
  Wire.endTransmission();
}

float readDS1621() {
  Wire.beginTransmission(DS1621_ADDR);
  Wire.write(START_CONVERT);
  Wire.endTransmission();

  delay(750);

  Wire.beginTransmission(DS1621_ADDR);
  Wire.write(READ_TEMP);
  Wire.endTransmission();

  Wire.requestFrom(DS1621_ADDR, 2);

  if (Wire.available() >= 2) {
    byte msb = Wire.read();
    byte lsb = Wire.read();

    float temp = msb;
    if (lsb & 0x80) temp += 0.5;
    if (msb & 0x80) temp -= 256;

    return temp;
  }
  return -999.9;
}

void loop() {
  tempDS1621 = readDS1621();
  tempBMP180 = bmp180.readTemperature();
  pressure = bmp180.readPressure() / 100.0;

  String data = "DS:" + String(tempDS1621,1) + ",BM:" + String(tempBMP180,1) + ",P:" + String(pressure,1);

  mySerial.println(data);   // envoi UART
  Serial.println("Envoi: " + data);

  delay(2000);
}
