

#include <A2a.h>
#include <ESP8266WiFi.h>
#include <LiquidCrystal_I2C.h>
#include "FirebaseESP8266.h"


#include "config.h"

#define slaveArduinoUnoAddress 0x08
#define displayI2CAddress 0x27
#define columnsDisplayI2C 16
#define rowsDisplayI2C 2
#define pinRelayInArduinoUno 9

A2a arduinoSlave;
FirebaseData firebaseData;

LiquidCrystal_I2C lcd(displayI2CAddress, columnsDisplayI2C, rowsDisplayI2C);

// Functions For Parts
void alterMessageDisplay();
void alterStatusRelay();
void alterStatusBackLightDisplay();

// Functions Setup
void startSlave();
void startLCD();
void startWifi();
void startFirebase();
String lastUpdateOnArduino = "test";



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (! Serial);
  arduinoSlave.begin(0, 2);
  startLCD();
  startSlave();
  startWifi();
  startFirebase();
  

}

void loop() {
  Firebase.getString(firebaseData, "lastUpdate");
  Serial.println(lastUpdateOnArduino);
  Serial.println(lastUpdateOnArduino != firebaseData.stringData());
  if (lastUpdateOnArduino != firebaseData.stringData()) {
    lastUpdateOnArduino=firebaseData.stringData();
    Serial.println("Atualizando");
    alterMessageDisplay();
    alterStatusRelay();
    alterStatusBackLightDisplay();

  }

}


void startSlave() {
  arduinoSlave.pinWireMode(slaveArduinoUnoAddress, pinRelayInArduinoUno, OUTPUT);
  arduinoSlave.digitalWireWrite(slaveArduinoUnoAddress, pinRelayInArduinoUno, LOW);
  lcd.print("------Slave-----");
  lcd.setCursor(0, 1);
  lcd.print("-------OK-------");
  delay(3000);
  lcd.clear();
}

void startLCD() {
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.print("Display OK!");
  delay(3000);
  lcd.clear();
}

void startWifi() {
  WiFi.mode(WIFI_STA);
  Serial.println("Conectando Wifi");
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    lcd.clear();
    lcd.print("Conectando Wifi");
    Serial.print(".");
    delay(500);
  }

  lcd.clear();
  lcd.print("------Wifi------");
  lcd.setCursor(0, 1);
  lcd.print("-------OK-------");
  lcd.clear();

  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
  lcd.print("-------IP-------");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());
  delay(3000);
  lcd.clear();


}

void startFirebase() {
  Serial.println("Conectando Firebase");
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  firebaseData.setBSSLBufferSize(1024, 1024);
  firebaseData.setResponseSize(1024);
  lcd.print("----Firebase----");
  lcd.setCursor(0, 1);
  lcd.print("-------OK-------");
  Serial.println("Firebase OK");
  delay(3000);
}


void alterMessageDisplay() {
  Serial.println("Chegou mensagem");

  Firebase.getString(firebaseData, "displayLCD/message");

  String textToPrint = firebaseData.stringData();
  lcd.setCursor(0, 0);

  if (textToPrint.length() <= 16) {
    lcd.clear();
    lcd.print(textToPrint);
  }
  else if (textToPrint.length() > 32) {
    lcd.clear();
    lcd.print("------Texto-----");
    lcd.setCursor(0, 1);
    lcd.print("--Muito Grande--");
  }
  else if (textToPrint.length() == 32) {
    lcd.clear();
    lcd.print(textToPrint.substring(0, 16));
    lcd.setCursor(0, 1);
    lcd.print(textToPrint.substring(16, 32));
  } else if ( textToPrint.length() < 32) {
    lcd.clear();
    lcd.print(textToPrint.substring(0, textToPrint.length() / 2));
    lcd.setCursor(0, 1);
    lcd.print(textToPrint.substring(textToPrint.length() / 2, textToPrint.length()));
  }
}

void alterStatusBackLightDisplay() {
  Serial.println("Chegou Backlight");
  Firebase.getString(firebaseData, "displayLCD/statusBackLight");
  String statusBackLight = firebaseData.stringData();
  if (statusBackLight == "on") {
    lcd.backlight();
  } else if (statusBackLight  == "off") {
    lcd.noBacklight();
  }
}

void alterStatusRelay() {
  Serial.println("Chegou Rele");
  Firebase.getString(firebaseData, "relayState/quarto");
  String statusRelay = firebaseData.stringData();
  if (statusRelay == "on") {
    arduinoSlave.digitalWireWrite(slaveArduinoUnoAddress, pinRelayInArduinoUno, HIGH);
  } else if (statusRelay  == "off") {
    arduinoSlave.digitalWireWrite(slaveArduinoUnoAddress, pinRelayInArduinoUno, LOW);
  }
}
