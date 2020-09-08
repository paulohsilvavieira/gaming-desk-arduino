#include <A2a.h>
#define slaveArduinoUnoAddress 0x08

A2a arduinoMaster;

void setup() {
  arduinoMaster.begin(slaveArduinoUnoAddress);
  arduinoMaster.onReceive(receiveDataByMaster);
  arduinoMaster.onRequest(sendDataByMaster);
}

void loop() {

}

void receiveDataByMaster() {
  arduinoMaster.receiveData(); 
}

void sendDataByMaster() {
  arduinoMaster.sendData(); 
}
