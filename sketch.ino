//12주차: 아두이노 <-> p5.js (가변저항 2개와 버튼값 송신 + 11주차 수신 유지)
#define LED 6
#define BUTTON 2
#define VARIABLE_R1 A1
#define VARIABLE_R2 A2
const int RGB[] = { 11, 5, 3 };  //Red 11, Green 5, Blue 3
int prev_value = 1;

void setup() {
  pinMode(LED, OUTPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(RGB[i], OUTPUT);
  }
  pinMode(BUTTON, INPUT);
  pinMode(VARIABLE_R1, INPUT);
  pinMode(VARIABLE_R2, INPUT);
  Serial.begin(9600);
}

void loop() {
  receiveData();
  sendData();
  delay(30);  //전송속도 지연
}

//p5.js에서 받기 (11주차): 동기신호 'A', 하늘 밝기, 시간대
void receiveData() {
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

//p5.js로 보내기 (12주차): "가변저항1,가변저항2,pressed" 형태의 문자열
void sendData() {
  int value1 = analogRead(VARIABLE_R1);
  int value2 = analogRead(VARIABLE_R2);
  Serial.print(value1);
  Serial.print(",");
  Serial.print(value2);
  Serial.print(",");

  int value3 = digitalRead(BUTTON);     //버튼값 읽기
  if (value3 == 0 && prev_value == 1) {  //버튼을 누른 순간에만 참이 됨
    Serial.println("pressed");
  } else {
    Serial.println("unpressed");
  }
  prev_value = value3;
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
