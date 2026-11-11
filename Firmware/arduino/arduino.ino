#include <Servo.h>

#define SERVO_LEFT_PIN       9
#define SERVO_RIGHT_PIN      10
#define RELAY_PUMP_PIN       8
#define BUZZER_PIN           7
#define FLAME_SENSOR_PIN     2

#define SCAN_MIN_ANGLE       20
#define SCAN_MAX_ANGLE       160
#define SCAN_STEP_DELAY      20
#define MIN_SPRAY_TIME       1500

Servo leftServo;
Servo rightServo;

int scanAngle = SCAN_MAX_ANGLE;
int scanDirection = -1;

void setup() {
  pinMode(RELAY_PUMP_PIN, OUTPUT);
  digitalWrite(RELAY_PUMP_PIN, HIGH);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(FLAME_SENSOR_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  leftServo.attach(SERVO_LEFT_PIN, 500, 2400);
  rightServo.attach(SERVO_RIGHT_PIN, 500, 2400);
  leftServo.write(SCAN_MAX_ANGLE);
  rightServo.write(SCAN_MAX_ANGLE);
  delay(500);
  Serial.println("SYS:READY");
}

void loop() {
  int flameState = digitalRead(FLAME_SENSOR_PIN);

  if (flameState == LOW) {
    leftServo.write(scanAngle);
    rightServo.write(scanAngle);
    digitalWrite(BUZZER_PIN, HIGH);
    Serial.println("FIRE_DETECTED");
    digitalWrite(RELAY_PUMP_PIN, LOW);

    delay(MIN_SPRAY_TIME);

    while (digitalRead(FLAME_SENSOR_PIN) == LOW) {
      delay(50);
    }

    digitalWrite(RELAY_PUMP_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);
    Serial.println("FIRE_CLEARED");

    while (digitalRead(FLAME_SENSOR_PIN) == LOW) {
      delay(100);
    }
    delay(500);

    leftServo.write(SCAN_MAX_ANGLE);
    rightServo.write(SCAN_MAX_ANGLE);
    scanAngle = SCAN_MAX_ANGLE;
    scanDirection = -1;
    delay(500);
    delay(1000);
  }
  else {
    scanAngle += scanDirection;

    if (scanAngle >= SCAN_MAX_ANGLE) {
      scanAngle = SCAN_MAX_ANGLE;
      scanDirection = -1;
    } else if (scanAngle <= SCAN_MIN_ANGLE) {
      scanAngle = SCAN_MIN_ANGLE;
      scanDirection = 1;
    }

    leftServo.write(scanAngle);
    rightServo.write(scanAngle);
    digitalWrite(RELAY_PUMP_PIN, HIGH);
    digitalWrite(BUZZER_PIN, LOW);
    delay(SCAN_STEP_DELAY);
  }
}