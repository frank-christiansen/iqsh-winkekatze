#include <Servo.h>

#define augenGPIO 2
#define armGPIO 12
#define armHoch 50
#define armRunter 180

Servo arm;

void setup() {
  pinMode(augenGPIO, OUTPUT);
  arm.attach(armGPIO);
  arm.write(armHoch);
  delay(2000);
}

void loop() {

  digitalWrite(augenGPIO, HIGH);
  delay(1000);
  digitalWrite(augenGPIO, LOW);
  delay(1000);

  arm.write(armRunter);
  delay(400);

  arm.write(armHoch);
  delay(10000);

}
