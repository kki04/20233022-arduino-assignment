void setup() {
  pinMode(7, OUTPUT);
}

void loop() {
  // 처음 1초 동안 LED 켜기
  digitalWrite(7, HIGH);
  delay(1000);

  // 다음 1초 동안 5번 깜빡이기
  for (int i = 0; i < 5; i++) {
    digitalWrite(7, LOW);
    delay(100);

    digitalWrite(7, HIGH);
    delay(100);
  }

  // LED 끄기
  digitalWrite(7, LOW);

  // 여기서 종료
  while (1) {
  }
}
