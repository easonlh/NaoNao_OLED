#include <Arduino.h>
#include <ESP32Servo.h>

#define SERVO_PIN 18

Servo servo;

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("========== SG90 Servo Diagnostic Test ==========");
  Serial.println();

  // Step 1: Basic init check
  Serial.println("[1] Initializing servo on GPIO18...");
  servo.setPeriodHertz(50);
  servo.attach(SERVO_PIN, 500, 2400);
  Serial.println("    Servo attached. Check: does the servo make any sound?");
  Serial.println("    If you hear a faint hum -> power OK");
  Serial.println("    If silent -> check wiring (Red=VIN, Brown=GND)");
  Serial.println();

  delay(2000);

  // Step 2: Sweep through microsecond values to find dead zone
  Serial.println("[2] Scanning PWM range to find dead zone (stop position)...");
  Serial.println("    This will take ~60 seconds. Watch the servo.");
  Serial.println("    If it spins at any point, note the value!");
  Serial.println();

  // Start from 700us, go to 2300us in 25us steps
  for (int us = 700; us <= 2300; us += 25) {
    servo.writeMicroseconds(us);
    Serial.printf("    PWM = %d us\n", us);
    delay(800);  // wait 800ms at each value
  }
  servo.writeMicroseconds(1500);  // back to center

  Serial.println();
  Serial.println("[3] Manual test (send single chars in serial monitor):");
  Serial.println("    '1' -> 1000us (should spin one direction)");
  Serial.println("    '2' -> 1200us (slow reverse)");
  Serial.println("    '3' -> 1400us (slow forward?)");
  Serial.println("    '4' -> 1500us (should STOP - center)");
  Serial.println("    '5' -> 1600us (slow reverse?)");
  Serial.println("    '6' -> 1800us (slow forward)");
  Serial.println("    '7' -> 2000us (should spin other direction)");
  Serial.println("    '8' -> 2200us (full speed other way)");
  Serial.println("    '0' -> detach servo");
  Serial.println();
  Serial.println("========== Ready ==========");
}

void loop() {
  if (Serial.available() > 0) {
    char cmd = Serial.read();
    switch (cmd) {
      case '1': servo.writeMicroseconds(1000); Serial.println("-> 1000us"); break;
      case '2': servo.writeMicroseconds(1200); Serial.println("-> 1200us"); break;
      case '3': servo.writeMicroseconds(1400); Serial.println("-> 1400us"); break;
      case '4': servo.writeMicroseconds(1500); Serial.println("-> 1500us (STOP)"); break;
      case '5': servo.writeMicroseconds(1600); Serial.println("-> 1600us"); break;
      case '6': servo.writeMicroseconds(1800); Serial.println("-> 1800us"); break;
      case '7': servo.writeMicroseconds(2000); Serial.println("-> 2000us"); break;
      case '8': servo.writeMicroseconds(2200); Serial.println("-> 2200us"); break;
      case '0': servo.detach(); Serial.println("Servo detached"); break;
    }
  }
}
