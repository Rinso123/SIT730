#define PORCH_LIGHT 10
#define HALLWAY_LIGHT 9

#define MOTION_SENSOR 2

#define SWITCH 3

const unsigned long PORCH_DURATION = 30 * 1000;
const unsigned long HALLWAY_DURATION = 60 * 1000;

void setup() {
  pinMode(PORCH_LIGHT, OUTPUT);
  pinMode(HALLWAY_LIGHT, OUTPUT);

  digitalWrite(PORCH_LIGHT, LOW);
  digitalWrite(HALLWAY_LIGHT, LOW);

  attachInterrupt(digitalPinToInterrupt(MOTION_SENSOR), handle_motion_detection, RISING);
  attachInterrupt(digitalPinToInterrupt(SWITCH), handle_switch_press, RISING);


  Serial.begin(9600);
}

volatile bool running = false;
volatile unsigned long start = 0;

void handle_switch_press() {
  Serial.println("Switch Pressed");
  start_sequence();
}

void handle_motion_detection() {
  Serial.println("Motion Detected");
  start_sequence();
}

void start_sequence() {
  running = true;
  start = millis();
  digitalWrite(HALLWAY_LIGHT, HIGH);
  digitalWrite(PORCH_LIGHT, HIGH);
}

void loop() {

  if (running) {
    if (millis() - start >= HALLWAY_DURATION) {
      digitalWrite(HALLWAY_LIGHT, LOW);
      start = 0;
      running = false;
    }

    if (millis() - start >= PORCH_DURATION) {
      digitalWrite(PORCH_LIGHT, LOW);
    }
  }
}
