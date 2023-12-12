#include "BluetoothSerial.h"
#include <ESP32Servo.h>
#include <Stepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution

// ULN2003 Motor Driver Pins
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

int stepM=4;

//#define USE_PIN // Uncomment this to use PIN during pairing. The pin is specified on the line below
const char *pin = "1234"; // Change this to more secure PIN.

String device_name = "Rebeca";

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run make menuconfig to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

int led=2;
BluetoothSerial SerialBT;
Servo servo;
int servoPin=32;


unsigned long previousMillis = 0;  // Declaración de la variable en el ámbito global
const long shortInterval = 20;  // Intervalo de tiempo en milisegundos
const long longInterval = 600000;  

Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(115200);
  SerialBT.begin(device_name); //Bluetooth device name
  Serial.printf("The device with name \"%s\" is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str());
  //Serial.printf("The device with name \"%s\" and MAC address %s is started.\nNow you can pair it with Bluetooth!\n", device_name.c_str(), SerialBT.getMacString()); // Use this after the MAC method is implemented
  #ifdef USE_PIN
    SerialBT.setPin(pin);
    Serial.println("Using PIN");
  #endif
  pinMode(led, OUTPUT);
  pinMode(stepM, OUTPUT);
  // set the speed at 5 rpm
  myStepper.setSpeed(15);
  servo.attach(servoPin);
}

void loop() {
  digitalWrite(led, HIGH);

  if (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  String receivedString;

  if (SerialBT.available()) {
    receivedString = SerialBT.readString(); // Read the entire string

    Serial.print("Received: ");
    Serial.println(receivedString);
  }

  Serial.print("Received: ");
  Serial.println(receivedString);

  if (receivedString == "baja") {
      servo.write(110);
  } else if (receivedString == "sube") {
      servo.write(0);
  }

  if (receivedString == "inclinar") {
      Serial.println("clockwise");
      myStepper.step(stepsPerRevolution*2-300);
  } else if (receivedString == "parar") {
      Serial.println("counterclockwise");
      myStepper.step(-stepsPerRevolution*2+300);
  }

  if(receivedString==""){
    servo.write(110);
    myStepper.step(stepsPerRevolution*2-300);
    delay(3000);
    myStepper.step(-stepsPerRevolution*2+300);
    servo.write(0);
    delay(600000);

  }

}
