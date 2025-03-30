#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <NewPing.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define SERVOMIN  150
#define SERVOMAX  600
#define USMIN     600
#define USMAX     2400
#define SERVO_FREQ 50
#define MAX_DISTANCE 200

// Motor pins for movement
const int motor1Pin1 = 6;
const int motor1Pin2 = 5;
const int motor2Pin1 = 7;
const int motor2Pin2 = 8;

// Define the PWM channels for the arm and the claw servos
const int ARM_SERVO_CHANNEL = 0;
const int CLAW_SERVO_CHANNEL = 1;

// Global variables for arm control
int currentArmPos = 500;
bool armClawClosed = false;

void adjustArm(int delta) {
  int newPos = currentArmPos + delta;
  
  if (newPos < SERVOMIN) {
    newPos = SERVOMIN;
  } else if (newPos > SERVOMAX) {
    newPos = SERVOMAX;
  }

  if (newPos != currentArmPos) {
    pwm.setPWM(ARM_SERVO_CHANNEL, 0, newPos);
    currentArmPos = newPos;

    delay(10);

    Serial.print("Arm adjusted to: ");
    Serial.println(currentArmPos);
  }
}


void toggleClaw() {
  if (armClawClosed) {
    for (uint16_t pulselen = 100; pulselen < 400; pulselen++) {
      pwm.setPWM(CLAW_SERVO_CHANNEL, 0, pulselen);
      delay(10);
    }
    armClawClosed = false;
    Serial.println("Claw opened");
  } else {
    for (uint16_t pulselen = 400; pulselen > 100; pulselen--) {
      pwm.setPWM(CLAW_SERVO_CHANNEL, 0, pulselen);
      delay(10);
    }
    armClawClosed = true;
    Serial.println("Claw closed");
  }
}

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
    pwm.setPWM(3, 0, (420 - pulselen) + 130);
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
    String command = Serial.readStringUntil('\n');
    command.trim();

    Serial.print("Received Command: ");
    Serial.println(command);

    if (command.startsWith("dir:")) {
      int dirValue = command.substring(4).toInt();
      handleDirection(dirValue);
    } 
    else if (command.startsWith("arm:")) {
      int armValue = command.substring(4).toInt();
      handleArm(armValue);
    } 
    else {
      Serial.println("Invalid command.");
    }
  }
}

void handleDirection(int dirValue) {
  switch (dirValue) {
    case 1:
      moveForward();
      break;
    case 2:
      moveRight();
      delay(500);
      stopMotors();
      break;
    case 3:
      stopMotors();
      Serial.println("Stopping all movements.");
      break;
    case 4:
      moveLeft();
      delay(500);
      stopMotors();
      break;
    default:
      Serial.println("Invalid direction command.");
      break;
  }
}

void handleArm(int armValue) {
  switch (armValue) {
    case 1:
      adjustArm(-50);
      break;
    case 2:
      adjustArm(50);
      break;
    case 3:
      toggleClaw();
      break;
    default:
      Serial.println("Invalid arm command.");
      break;
  }
}
