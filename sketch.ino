//13주차: 아두이노 -> p5.js 핸드셰이킹 (p5.js가 'B'로 요청할 때만 센서값 송신, 11주차 수신 유지)
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
  while (Serial.available() == 0) {  //수신 버퍼에 수신값이 없는 동안 반복 실행
    Serial.println('A');             //시작신호 전송
    delay(300);                      //0.3초 대기
  }
}

void loop() {
  while (Serial.available() > 0) {
    int value = Serial.read();  //수신값 읽기
    if (value == 'B') {         //p5.js의 요청 신호
      sendData();
    } else if (value == 'A') {  //p5.js의 하늘 정보 (11주차)
      receiveData();
    }
  }
}

//p5.js에서 받기 (11주차): 동기신호 'A' 다음의 하늘 밝기, 시간대
void receiveData() {
  while (Serial.available() < 2) {  //나머지 2바이트가 올 때까지 대기
    delay(1);                       //1ms 쉬면서 기다리기
  }
  int light = Serial.read();  //하늘 밝기 (0 ~ 255)
  int phase = Serial.read();  //시간대 (0 = 낮, 1 = 노을, 2 = 밤)
  analogWrite(LED, light);
  setPhaseColor(phase);
}

//p5.js로 보내기 (12주차): "가변저항1,가변저항2,pressed" 형태의 문자열
void sendData() {
  int value1 = analogRead(VARIABLE_R1);
  int value2 = analogRead(VARIABLE_R2);
  Serial.print(value1);
  Serial.print(",");
  Serial.print(value2);
  Serial.print(",");

  int value3 = digitalRead(BUTTON);      //버튼값 읽기
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
