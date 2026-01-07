#include <SPI.h>
#include <SD.h>

const int chipSelect = 4;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; // attendre l'ouverture du moniteur série
  }

  Serial.println("Initialisation de la carte SD...");

  if (!SD.begin(chipSelect)) {
    Serial.println("❌ Carte SD non détectée ou problème de connexion.");
    return;
  }

  Serial.println("✅ Carte SD détectée avec succès.");

  // Création / écriture dans un fichier
  File testFile = SD.open("test.txt", FILE_WRITE);

  if (testFile) {
    Serial.println("✍️ Écriture dans test.txt...");
    testFile.println("Bonjour, la carte SD fonctionne !");
    testFile.close();
    Serial.println("✅ Écriture terminée.");
  } else {
    Serial.println("❌ Impossible d'écrire dans le fichier.");
    return;
  }

  // Lecture du fichier
  testFile = SD.open("test.txt");
  if (testFile) {
    Serial.println("📖 Lecture du fichier test.txt :");
    while (testFile.available()) {
      Serial.write(testFile.read());
    }
    testFile.close();
  } else {
    Serial.println("❌ Impossible de lire le fichier.");
  }
}

void loop() {
  // Rien à faire ici
}