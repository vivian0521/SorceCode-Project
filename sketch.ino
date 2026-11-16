//11주차: p5.js -> 아두이노 (복수 데이터 수신)
#define LED 6
const int RGB[] = { 11, 5, 3 };  //Red 11, Green 5, Blue 3

void setup() {
  pinMode(LED, OUTPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(RGB[i], OUTPUT);
  }
  Serial.begin(9600);
  analogWrite(LED, 255);
}

void loop() {
  while (Serial.available() >= 3) {  //수신 버퍼에 3바이트 이상 있을 때
    int value = Serial.read();       //수신값 읽기
    if (value == 'A') {              //동기신호 확인
      int light = Serial.read();     //하늘 밝기 (0 ~ 255)
      int phase = Serial.read();     //시간대 (0 = 낮, 1 = 노을, 2 = 밤)
      analogWrite(LED, light);
      setPhaseColor(phase);
    }
  }
}

//시간대에 맞는 RGB 색 출력 (밝기 100 이하로 고정)
void setPhaseColor(int phase) {
  switch (phase) {
    case 0:  //낮: 노랑
      setRGB(100, 80, 0);
      break;
    case 1:  //노을: 주황
      setRGB(100, 30, 0);
      break;
    case 2:  //밤: 파랑
      setRGB(0, 0, 100);
      break;
  }
}

void setRGB(int r, int g, int b) {
  analogWrite(RGB[0], r);
  analogWrite(RGB[1], g);
  analogWrite(RGB[2], b);
}
