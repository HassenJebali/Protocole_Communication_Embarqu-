#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <Adafruit_BMP085.h>

// --------- SoftwareSerial ---------
SoftwareSerial mySerial(10, 11);  // RX, TX

// --------- LCD 16x2 I2C ---------
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int ledPin = 5;
const int ledPin0 = 6;

String receivedData;
float previousPressure = 0;
float airflowThreshold = 0.1;  // ΔP en hPa pour détecter le courant d'air
float tempThreshold = 22.0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("En attente...");

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  if (mySerial.available()) {
    String receivedData = mySerial.readStringUntil('\n');
    receivedData.trim();

    Serial.println("Reçu: " + receivedData);

    // Vérification format attendu : "DS:val,BM:val,P:val"
    int dsIndex = receivedData.indexOf("DS:");
    int bmIndex = receivedData.indexOf(",BM:");
    int pIndex  = receivedData.indexOf(",P:");

    if (dsIndex != -1 && bmIndex != -1 && pIndex != -1) {
      String dsVal = receivedData.substring(dsIndex + 3, bmIndex);
      String bmVal = receivedData.substring(bmIndex + 4, pIndex);
      String pVal  = receivedData.substring(pIndex + 3);
      float pressure = pVal.toFloat();
      float tempDS = dsVal.toFloat();

      // Détection courant d'air
      if (previousPressure != 0 && abs(previousPressure - pressure) >= airflowThreshold) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Courant d'air");
        digitalWrite(ledPin, HIGH);  // LED rouge ON
        mySerial.println("ALERTE: AIRFLOW DETECTED!");
        delay(5000);
        digitalWrite(ledPin, LOW);   // LED rouge OFF
        lcd.clear();
      } 
      previousPressure = pressure;

      if (tempDS > tempThreshold) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("HIGH TEMP!");
        lcd.setCursor(0,1);
        lcd.print(String(tempDS) + " C");

        // LED clignotante
        for (int i = 0; i < 5; i++) {
          digitalWrite(ledPin0, HIGH);
          delay(300);
          digitalWrite(ledPin0, LOW);
          delay(300);
        }

        mySerial.println("ALERTE: HIGH TEMP! " + String(tempDS) + " C");
        lcd.clear();
      }

      // --- Affichage DS1621 ---
      lcd.setCursor(0,0);
      lcd.print("DS1621:");
      lcd.setCursor(0,1);
      lcd.print(dsVal + " C");
      delay(2000);

      // --- Affichage séparation ---
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("----");
      delay(500);

      // --- Affichage BMP180 ---
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("BMP180:");
      lcd.setCursor(0,1);
      lcd.print(bmVal + " C");
      delay(2000);

      // --- Affichage séparation ---
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("----");
      delay(500);

      // --- Affichage Pression ---
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Pression:");
      lcd.setCursor(0,1);
      lcd.print(pVal + " hPa");
      delay(2000);

      // --- Affichage séparation finale ---
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("----");
      delay(500);
    }
  }
}
