/*
 * Autonomous Robotic Vacuum Cleaner - ESP32 v2.0.17
 * Features:
 * - Autonomous navigation with ultrasonic + IR sensors
 * - MOSFET-controlled vacuum
 * - Start/Stop control via Dabble App
 * - MOSFET safety monitoring
 * Note: No battery ADC used. Safe for 9V battery input via Vin.
 */

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <DabbleESP32.h>

// Motor Driver L298N Pins
#define IN1 25
#define IN2 26
#define IN3 27
#define IN4 14
#define ENA 32
#define ENB 33

// Ultrasonic Sensors
#define TRIG_FRONT 2
#define ECHO_FRONT 4
#define TRIG_LEFT 13
#define ECHO_LEFT 35
#define TRIG_RIGHT 5
#define ECHO_RIGHT 18

// IR Sensors (Cliff Detection)
#define IR_LEFT 19
#define IR_CENTER 21
#define IR_RIGHT 22

// Vacuum Motor (MOSFET Controlled)
#define VACUUM_MOTOR 12
#define MOSFET_STATUS_PIN 34   // Optional analog feedback for MOSFET

// PWM Configuration
const int pwmFreq = 5000;
const int pwmResolution = 8;
const int pwmChannelENA = 0;
const int pwmChannelENB = 1;

// System Variables
bool systemActive = false;
int motorSpeed = 200;
int turnSpeed = 180;
int safeDistance = 15;
bool wallFollowMode = true;
unsigned long lastSafetyCheck = 0;

// ------------------- SETUP -------------------
void setup() {
  Serial.begin(115200);
  Dabble.begin("VacuumBot");

  // PWM setup
  ledcSetup(pwmChannelENA, pwmFreq, pwmResolution);
  ledcSetup(pwmChannelENB, pwmFreq, pwmResolution);
  ledcAttachPin(ENA, pwmChannelENA);
  ledcAttachPin(ENB, pwmChannelENB);

  // Pin modes
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(VACUUM_MOTOR, OUTPUT);
  pinMode(MOSFET_STATUS_PIN, INPUT);

  pinMode(TRIG_FRONT, OUTPUT); pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_LEFT, OUTPUT); pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT); pinMode(ECHO_RIGHT, INPUT);

  pinMode(IR_LEFT, INPUT); pinMode(IR_CENTER, INPUT); pinMode(IR_RIGHT, INPUT);

  stopMotors();
  digitalWrite(VACUUM_MOTOR, LOW);

  Serial.println("========================================");
  Serial.println("ESP32 Vacuum Cleaner v2.0.17 (Autonomous + MOSFET Safe)");
  Serial.println("START = Begin Cleaning | SELECT = Stop All");
  Serial.println("========================================");
}

// ------------------- MAIN LOOP -------------------
void loop() {
  Dabble.processInput();

  // START button
  if (GamePad.isStartPressed()) {
    if (!systemActive) {
      systemActive = true;
      digitalWrite(VACUUM_MOTOR, HIGH);
      Serial.println("✅ SYSTEM STARTED - Vacuum & Autonomous Mode ON");
      delay(300);
    }
  }

  // STOP button
  if (GamePad.isSelectPressed()) {
    if (systemActive) {
      systemActive = false;
      stopMotors();
      digitalWrite(VACUUM_MOTOR, LOW);
      Serial.println("🛑 SYSTEM STOPPED - Motors & Vacuum OFF");
      delay(300);
    }
  }

  // Run only if active
  if (systemActive) {
    // MOSFET safety check every 1s
    if (millis() - lastSafetyCheck > 1000) {
      checkSafety();
      lastSafetyCheck = millis();
    }

    // Autonomous navigation
    if (systemActive) autonomousNavigation();
  } else {
    stopMotors();
    digitalWrite(VACUUM_MOTOR, LOW);
  }

  delay(50);
}

// ------------------- SAFETY CHECK -------------------
void checkSafety() {
  int mosfetLoad = analogRead(MOSFET_STATUS_PIN);

  if (mosfetLoad > 3500) {  // adjust threshold for your circuit
    Serial.println("⚠ MOSFET OVERLOAD - Shutting Down System");
    systemActive = false;
    stopMotors();
    digitalWrite(VACUUM_MOTOR, LOW);
  }

  Serial.print("MOSFET Load: "); Serial.println(mosfetLoad);
}

// ------------------- AUTONOMOUS NAVIGATION -------------------
void autonomousNavigation() {
  int distFront = getUltrasonicDistance(TRIG_FRONT, ECHO_FRONT);
  int distLeft = getUltrasonicDistance(TRIG_LEFT, ECHO_LEFT);
  int distRight = getUltrasonicDistance(TRIG_RIGHT, ECHO_RIGHT);

  bool irLeft = digitalRead(IR_LEFT);
  bool irCenter = digitalRead(IR_CENTER);
  bool irRight = digitalRead(IR_RIGHT);

  // Cliff detection
  if (irLeft == LOW || irCenter == LOW || irRight == LOW) {
    Serial.println("⚠ CLIFF DETECTED!");
    handleCliffDetection(irLeft, irCenter, irRight);
    return;
  }

  // Obstacle avoidance
  if (distFront < safeDistance) {
    handleFrontObstacle(distLeft, distRight);
  } else if (distLeft < safeDistance) {
    turnRight(30);
  } else if (distRight < safeDistance) {
    turnLeft(30);
  } else {
    moveForward(motorSpeed);

    // Wall following
    if (wallFollowMode) {
      if (distLeft < 40 && distLeft > safeDistance) {
        setMotorSpeed(motorSpeed - 30, motorSpeed + 30);
      } else if (distRight < 40 && distRight > safeDistance) {
        setMotorSpeed(motorSpeed + 30, motorSpeed - 30);
      }
    }
  }
}

// ------------------- SENSOR + MOTOR FUNCTIONS -------------------
int getUltrasonicDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000);
  int distance = duration * 0.034 / 2;
  return (distance == 0 || distance > 200) ? 200 : distance;
}

void setMotorSpeed(int leftSpeed, int rightSpeed) {
  ledcWrite(pwmChannelENA, leftSpeed);
  ledcWrite(pwmChannelENB, rightSpeed);
}

void moveForward(int speed) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setMotorSpeed(speed, speed);
}

void moveBackward(int speed) {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  setMotorSpeed(speed, speed);
}

void turnLeft(int degrees) {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
  setMotorSpeed(turnSpeed, turnSpeed);
  delay(degrees * 6);
  stopMotors();
}

void turnRight(int degrees) {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
  setMotorSpeed(turnSpeed, turnSpeed);
  delay(degrees * 6);
  stopMotors();
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  setMotorSpeed(0, 0);
}

// ------------------- CLIFF & OBSTACLE HANDLERS -------------------
void handleCliffDetection(bool left, bool center, bool right) {
  stopMotors();
  delay(200);
  moveBackward(150);
  delay(600);
  stopMotors();
  delay(200);

  if (center == LOW) turnRight(180);
  else if (left == LOW) turnRight(90);
  else if (right == LOW) turnLeft(90);
}

void handleFrontObstacle(int distLeft, int distRight) {
  stopMotors();
  delay(200);
  moveBackward(150);
  delay(500);
  stopMotors();
  delay(200);

  if (distLeft > distRight + 10) turnLeft(90);
  else turnRight(90);
}


