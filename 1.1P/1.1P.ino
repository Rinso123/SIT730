#define PORCH_LIGHT 8
#define HALLWAY_LIGHT 9

#define SWITCH 4

#define PORCH_DURATION 30 * 1000u
#define HALLWAY_DURATION 60 * 1000u

void setup() {
  pinMode(PORCH_LIGHT, OUTPUT);
  pinMode(HALLWAY_LIGHT, OUTPUT);
  pinMode(SWITCH, INPUT_PULLUP);

  digitalWrite(PORCH_LIGHT, LOW);
  digitalWrite(HALLWAY_LIGHT, LOW);

}

bool running = false;
unsigned  long start = 0;

void loop() {
  
  if (digitalRead(SWITCH) == LOW) {
    if (!running) {
      running = true;
      start = millis();
      digitalWrite(PORCH_LIGHT, HIGH);
      digitalWrite(HALLWAY_LIGHT, HIGH);
      delay(500);
    }
  }

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
