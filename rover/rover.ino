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

const int triggerPin = 3;
const int echoPin = 4;

long distance = 100;
uint8_t servonum = 0;
int return_code = -1;

//0 - left/right
//1 - pick up/put down
//2 - forth/back
//3 - up/down
void movement(int max_front){
  //rotate front
  for (uint16_t pulselen = 600; pulselen > max_front; pulselen--) {
    pwm.setPWM(0, 0, pulselen);
    delay(10);
  }
  delay(1000);

  //forth
  pwm.setPWM(3, 0, 130);
  for (uint16_t pulselen = 250; pulselen < 500; pulselen++) {
    pwm.setPWM(2, 0, pulselen);
    delay(10);
  }
  delay(1000);

  //close the hand
  for (uint16_t pulselen = 400; pulselen > 100; pulselen--) {
    pwm.setPWM(1, 0, pulselen);
    delay(10);
  }
  delay(500);

  //back   
  for (uint16_t pulselen = 420; pulselen > 250; pulselen--) {
    pwm.setPWM(2, 0, pulselen);
    pwm.setPWM(3, 0, (420-pulselen)+130);
    delay(10);
  }
  delay(1000);


  //rotate back
  for (uint16_t pulselen = max_front; pulselen < 600; pulselen++) {
    pwm.setPWM(0, 0, pulselen);
    delay(10);
  }
  delay(1000);

  //open the hand
  for (uint16_t pulselen = 100; pulselen < 400; pulselen++) {
    pwm.setPWM(1, 0, pulselen);
    delay(10);
  }
}


void setServoPulse(uint8_t n, double pulse) {
  double pulselength;
  pulselength = 1000000;   // 1,000,000 us per second
  pulselength /= SERVO_FREQ;   // Analog servos run at ~60 Hz updates
  Serial.print(pulselength); Serial.println(" us per period"); 
  pulselength /= 4096;  // 12 bits of resolution
  Serial.print(pulselength); Serial.println(" us per bit"); 
  pulse *= 1000000;  // convert input seconds to us
  pulse /= pulselength;
  Serial.println(pulse);
  pwm.setPWM(n, 0, pulse);

}

long measureDistance() {
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  return pulseIn(echoPin, HIGH) * 0.034 / 2; // Calculate distance in centimeters
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

int checkingObstacle(){
  if (Serial.available() > 0) {
    // char received = Serial.read();
    int received = Serial.parseInt();
    Serial.print("Arduino received ");
    Serial.println(received);
    delay(1000);

    return received;
  }
  return -1;
} 


void setup() {
  Serial.begin(9600);

  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);

  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(SERVO_FREQ);  // Analog servos run at ~50 Hz updates
  delay(1000);

  //pwm.setPWM(0, 0, 600);
  //pwm.setPWM(1, 0, 400);
  //pwm.setPWM(2, 0, 250);
  //pwm.setPWM(3, 0, 300);
}


void loop() {
  moveForward();
  distance = measureDistance();
  if(distance < 10){
    stopMotors();
    delay(1000);
    Serial.println("Obstacle found! Checking what it is...");
    // delay(100);
    // while(1){
    //   return_code = checkingObstacle();
    //   if(return_code != -1)break;
    // }

    // if(return_code == 0){
    //   moveRight();
    //   delay(500);
    //   stopMotors();
    // }else if(return_code == 1){
      //max_front -> from raspberry pi 
      //range - from 150 to 0  (most right to most left possible)
      movement(150);
      delay(1000);
    // }
    delay(500);
  }

  //can be added if there is no object - to go left or right
  // if(case == 2)moveLeft();
  // if(case == 3)moveRight();

  moveForward();
}

