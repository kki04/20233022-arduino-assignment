// Arduino pin assignment
#define PIN_LED 9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0
#define INTERVAL 25
#define PULSE_DURATION 10
#define _DIST_MIN 100.0
#define _DIST_MAX 300.0

#define TIMEOUT ((INTERVAL / 2) * 1000.0)
#define SCALE (0.001 * 0.5 * SND_VEL)

unsigned long last_sampling_time;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);

  digitalWrite(PIN_TRIG, LOW);

  Serial.begin(57600);
}

void loop() {

  float distance;
  int duty;

  // sampling every 25 ms
  if (millis() - last_sampling_time < INTERVAL) {
    return;
  }

  distance = USS_measure(PIN_TRIG, PIN_ECHO);

  // LED brightness control
  if ((distance == 0.0) || (distance > _DIST_MAX)) {

    distance = _DIST_MAX + 10.0;
    duty = 255;                       // LED OFF

  }
  else if (distance < _DIST_MIN) {

    distance = _DIST_MIN - 10.0;
    duty = 255;                       // LED OFF

  }
  else if (distance <= 200.0) {

    // 100mm : OFF
    // 150mm : 50%
    // 200mm : MAX
    duty = 255.0 * (200.0 - distance) / 100.0;

  }
  else {

    // 200mm : MAX
    // 250mm : 50%
    // 300mm : OFF
    duty = 255.0 * (distance - 200.0) / 100.0;

  }

  analogWrite(PIN_LED, duty);


  // output to serial port
  Serial.print("Min:");
  Serial.print(_DIST_MIN);

  Serial.print(",distance:");
  Serial.print(distance);

  Serial.print(",Max:");
  Serial.print(_DIST_MAX);

  Serial.print(",duty:");
  Serial.print(duty);

  Serial.println("");


  // update next sampling time
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
