const int k1 = 23;
const int led = 1;

int key_state = 0;

void setup() {
  pinMode(k1, INPUT);
  pinMode(led, OUTPUT);
}

void loop()
{
  key_state = digitalRead(k1);
  digitalWrite(led, key_state);
}
