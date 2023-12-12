#include <Arduino.h>
#include "BluetoothSerial.h"

const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "RebecaP2";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

BluetoothSerial SerialBT;
// change this to whichever pin you want to use
int buzzer = 18;

int sensorValue = 0;
int pinLed=2;
int pinEco=12;
int pinGatillo=13;
int ledG=14;
int ledB=27;
int ledR=26;
int ledY=25;

boolean mood;
String op;
// notes of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes

long readUltrasonicDistance(int triggerPin, int echoPin)
{
  //Iniciamos el pin del emisor de reuido en salida
  pinMode(triggerPin, OUTPUT);
  //Apagamos el emisor de sonido
  digitalWrite(triggerPin, LOW);
  //Retrasamos la emision de sonido por 2 milesismas de segundo
  delayMicroseconds(2);
  // Comenzamos a emitir sonido
  digitalWrite(triggerPin, HIGH);
  //Retrasamos la emision de sonido por 2 milesismas de segundo
  delayMicroseconds(10);
  //Apagamos el emisor de sonido
  digitalWrite(triggerPin, LOW);
  //Comenzamos a escuchar el sonido
  pinMode(echoPin, INPUT);
  // Calculamos el tiempo que tardo en regresar el sonido
  return pulseIn(echoPin, HIGH);
}

void setup() {
  Serial.begin(115200);

  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  pinMode(ledR, OUTPUT);
  pinMode(ledY, OUTPUT);

  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif
}

void loop() {
  sensorValue = 0.01723 * readUltrasonicDistance(pinGatillo, pinEco);

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  String receivedString;

  if (SerialBT.available()) {
    receivedString = SerialBT.readString();
    Serial.print("Received: ");
    Serial.println(receivedString);
  }

  if (receivedString == "song 1" || receivedString == "song 2" || receivedString == "song 3" || receivedString == "song 4") {
    op = receivedString;
  }else if(receivedString == "stop"){
    op="";
  }


  if (op == "song 1") {
    jingleBells(buzzer);
  } else if (op == "song 2") {
    songSC(buzzer);
  } else if (op == "song 3") {
    songHpPlay(buzzer);
  } else if (op == "song 4") {
    song(buzzer);
  }

  if(receivedString=="sensorD"){
    mood=true;
  }else if(receivedString=="manual"){
    mood=false;
  }

  if (mood) {
    Serial.print("Valor del sensor de proximidad: ");
    Serial.println(sensorValue);
    if (sensorValue <= 20) {
      jingleBells(buzzer);
    }else if(sensorValue <= 50){
      songSC(buzzer);
    }else if(sensorValue <= 75){
      songHpPlay(buzzer);
    }else if(sensorValue <= 100){
      song(buzzer);
    }else{
      stopPlayback();
    }
  }
}

void stopPlayback() {
  noTone(buzzer); // Detiene la reproducción del tono actual
  // Puedes agregar aquí cualquier lógica adicional para detener la reproducción.
}
