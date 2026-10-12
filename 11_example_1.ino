
#include <Servo.h>

// Arduino pin assignment
#define PIN_LED   9   // LED active-low
#define PIN_TRIG  12  // sonar sensor TRIGGER
#define PIN_ECHO  13  // sonar sensor ECHO
#define PIN_SERVO 10  // servo motor

// configurable parameters for sonar
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 180.0
#define _DIST_MAX 360.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

#define _EMA_ALPHA 0.05

#define _DUTY_MIN 555
#define _DUTY_NEU ((_DUTY_MIN + _DUTY_MAX) / 2)
#define _DUTY_MAX 2393

// global variables
float dist_ema, dist_prev = _DIST_MIN;
unsigned long last_sampling_time;

Servo myservo;

bool ema_initialized = false;

void setup() {
  // initialize GPIO pins
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);
  digitalWrite(PIN_LED, HIGH);

  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);

  // initialize USS related variables
  dist_prev = _DIST_MIN;

  // initialize serial port
  Serial.begin(57600);
}

void loop() {
  float dist_raw, dist_filtered;

  // wait until next sampling time
  if (millis() < last_sampling_time + INTERVAL)
    return;

  // get a distance reading from the USS
  dist_raw = USS_measure(PIN_TRIG, PIN_ECHO);

  if (dist_raw == 0.0) {
    dist_filtered = dist_prev;
  }
  else {
    // 실제 측정 거리는 180~360mm로 제한
    dist_filtered = constrain(dist_raw, _DIST_MIN, _DIST_MAX);
    dist_prev = dist_filtered;
  }

  // LED control
  // 측정에 성공하고 180~360mm 범위일 때 ON
  if (dist_raw >= _DIST_MIN && dist_raw <= _DIST_MAX) {
    digitalWrite(PIN_LED, LOW);
  }
  else {
    digitalWrite(PIN_LED, HIGH);
  }

  if (!ema_initialized) {
    dist_ema = dist_filtered;
    ema_initialized = true;
  }
  else {
    dist_ema = _EMA_ALPHA * dist_filtered
             + (1.0 - _EMA_ALPHA) * dist_ema;
  }

  float ratio = (dist_ema - _DIST_MIN)
              / (_DIST_MAX - _DIST_MIN);

  ratio = constrain(ratio, 0.0, 1.0);

  int duty = _DUTY_MIN
           + ratio * (_DUTY_MAX - _DUTY_MIN);

  myservo.writeMicroseconds(duty);

  Serial.print("Min:");    Serial.print(_DIST_MIN);
  Serial.print(",dist:");  Serial.print(dist_raw);
  Serial.print(",ema:");   Serial.print(dist_ema);
  Serial.print(",Servo:"); Serial.print(myservo.read());
  Serial.print(",Max:");   Serial.print(_DIST_MAX);
  Serial.println("");

  // update last sampling time
  last_sampling_time += INTERVAL;
}

// get a distance reading from USS
float USS_measure(int TRIG, int ECHO)
{
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(TRIG, LOW);

  return pulseIn(ECHO, HIGH, TIMEOUT) * SCALE;
}
