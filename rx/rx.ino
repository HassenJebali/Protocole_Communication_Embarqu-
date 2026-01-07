///RX avant buzzer modifier


#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
#include <SD.h>
#include <SPI.h>

// --------- SoftwareSerial ---------
SoftwareSerial mySerial(2, 3);  // RX, TX

// --------- LCD 16x2 I2C ---------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// --------- SD ---------
const int chipSelect = 4;
File dataFile;

// --------- LEDs ---------
const int ledPin = 8;
const int ledPin0 = 9;

// --------- Variables ---------
float previousPressure = 0;
float airflowThreshold = 0.1;  // hPa
float tempThreshold = 18.0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Init SD...");

  pinMode(ledPin, OUTPUT);
  pinMode(ledPin0, OUTPUT);

  // ---------- INIT SD ----------
  if (!SD.begin(chipSelect)) {
    lcd.clear();
    lcd.print("SD FAIL");
    Serial.println("❌ Carte SD absente !");
    while (1);
  }

  lcd.clear();
  lcd.print("SD OK");
  delay(1000);

  lcd.clear();
  lcd.print("En attente...");
}

// ===== ENREGISTREMENT SD =====
void saveToSD(String text) {
  dataFile = SD.open("data.txt", FILE_WRITE);
  if (dataFile) {
    dataFile.println(text);
    dataFile.close();
    Serial.println("💾 SD: " + text);
  } else {
    Serial.println("❌ Erreur ecriture SD");
  }
}

void loop() {
  if (mySerial.available()) {
    String receivedData = mySerial.readStringUntil('\n');
    receivedData.trim();

    Serial.println("Reçu: " + receivedData);

    // Format attendu : DS:x,BM:x,P:x
    int dsIndex = receivedData.indexOf("DS:");
    int bmIndex = receivedData.indexOf(",BM:");
    int pIndex  = receivedData.indexOf(",P:");

    if (dsIndex != -1 && bmIndex != -1 && pIndex != -1) {
      String dsVal = receivedData.substring(dsIndex + 3, bmIndex);
      String bmVal = receivedData.substring(bmIndex + 4, pIndex);
      String pVal  = receivedData.substring(pIndex + 3);

      float pressure = pVal.toFloat();
      float tempDS = dsVal.toFloat();

      // ---------- SAUVEGARDE SD ----------
      saveToSD(receivedData);

      // ---------- DETECTION AIRFLOW ----------
      if (previousPressure != 0 && abs(previousPressure - pressure) >= airflowThreshold) {
        lcd.clear();
        lcd.print("Courant d'air");
        digitalWrite(ledPin, HIGH);
        mySerial.println("ALERTE: AIRFLOW!");
        delay(5000);
        digitalWrite(ledPin, LOW);
        lcd.clear();
      }
      previousPressure = pressure;

      // ---------- TEMP ELEVEE ----------
      if (tempDS > tempThreshold) {
        lcd.clear();
        lcd.print("HIGH TEMP!");
        lcd.setCursor(0, 1);
        lcd.print(tempDS, 1);
        lcd.print(" C");

        for (int i = 0; i < 5; i++) {
          digitalWrite(ledPin0, HIGH);
          delay(300);
          digitalWrite(ledPin0, LOW);
          delay(300);
        }

        mySerial.println("ALERTE: TEMP " + String(tempDS, 1));
        lcd.clear();
      }

      // ---------- AFFICHAGE ----------
      lcd.setCursor(0, 0);
      lcd.print("DS1621:");
      lcd.setCursor(0, 1);
      lcd.print(dsVal + " C");
      delay(2000);

      lcd.clear();
      lcd.print("----");
      delay(500);

      lcd.clear();
      lcd.print("BMP180:");
      lcd.setCursor(0, 1);
      lcd.print(bmVal + " C");
      delay(2000);

      lcd.clear();
      lcd.print("----");
      delay(500);

      lcd.clear();
      lcd.print("Pression:");
      lcd.setCursor(0, 1);
      lcd.print(pVal + " hPa");
      delay(2000);

      lcd.clear();
      lcd.print("----");
      delay(500);
    }
  }
}
