#include <Wire.h>
#include <Adafruit_PWMServoDriver.h> 
#include <NewPing.h>


Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
#define SERVOMIN  150 // This is the 'minimum' pulse length count (out of 4096)
#define SERVOMAX  600 // This is the 'maximum' pulse length count (out of 4096)
#define USMIN  600 // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX  2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600
#define SERVO_FREQ 50 // Analog servos run at ~50 Hz updates
#define MAX_DISTANCE 200 

const int motor1Pin1 = 6;
const int motor1Pin2 = 5;
const int motor2Pin1 = 7;
const int motor2Pin2 = 8;

void movement(int max_front) {
  for (uint16_t pulselen = 600; pulselen > max_front; pulselen--) {
    pwm.setPWM(0, 0, pulselen);
    delay(10);
  }
  delay(1000);

  pwm.setPWM(3, 0, 130);
  for (uint16_t pulselen = 250; pulselen < 500; pulselen++) {
    pwm.setPWM(2, 0, pulselen);
    delay(10);
  }
  delay(1000);

  for (uint16_t pulselen = 400; pulselen > 100; pulselen--) {
    pwm.setPWM(1, 0, pulselen);
    delay(10);
  }
  delay(500);

  for (uint16_t pulselen = 420; pulselen > 250; pulselen--) {
    pwm.setPWM(2, 0, pulselen);
    pwm.setPWM(3, 0, (420-pulselen) + 130);
    delay(10);
  }
  delay(1000);

  for (uint16_t pulselen = max_front; pulselen < 600; pulselen++) {
    pwm.setPWM(0, 0, pulselen);
    delay(10);
  }
  delay(1000);

  for (uint16_t pulselen = 100; pulselen < 400; pulselen++) {
    pwm.setPWM(1, 0, pulselen);
    delay(10);
  }
}

void moveForward() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void moveRight() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void moveLeft() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void stopMotors() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}

void setup() {
  Serial.begin(9600);

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);
  delay(1000);
}

void loop() {
  if (Serial.available() > 0) {
    int command = Serial.parseInt();
    Serial.print("Received Command: ");
    Serial.println(command);

    switch (command) {
      case 1:
        moveForward();
        break;
      case 2:
        moveRight();
        delay(500);
        stopMotors();
        break;
      case 4:
        moveLeft();
        delay(500);
        stopMotors();
        break;
      case 3:
        stopMotors();
        movement(150);
        break;
      case -1:
        stopMotors();
        Serial.println("Stopping all movements.");
        break;
      default:
        Serial.println("Invalid command.");
        break;
    }
  }
}