//7주차 v.3
#define BTN1 4
#define BTN2 2
#define VAR_L A1
#define VAR_R A2
//핀 번호 지정
const int LED[] = { 12, 8, 7, 6 };
const int RGB[] = { 11, 5, 3 };  //Red 11, Green 5, Blue 3

#define LED_NUM 4
#define RGB_NUM 3
#define BLINK_MS 500

//state (설정값은 OFF 모드에서도 유지되므로 별도 백업이 필요 없음)
int ledVal[4] = { 0, 0, 0, 0 };  //0 / 1
int rgbVal[3] = { 0, 0, 0 };     //0 ~ 255

int mode = 0;            //버튼1: 0 = OFF, 1 = ON, 2 = 점멸
boolean useRGB = false;  //버튼2: false = LED, true = RGB
int prev_value1 = 1, prev_value2 = 1;
int lastSlot = -1, lastVal = -1;
boolean blinkState = true;
unsigned long current_time, prev_time;

void setup() {
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
  pinMode(VAR_L, INPUT);
  pinMode(VAR_R, INPUT);
  Serial.begin(9600);

  //LED 4개 순서대로 깜빡
  for (int i = 0; i < LED_NUM; i++) {
    pinMode(LED[i], OUTPUT);
    digitalWrite(LED[i], HIGH);
    delay(100);
    digitalWrite(LED[i], LOW);
    delay(100);
  }
  //RGB 깜빡 (밝기 100)
  for (int i = 0; i < RGB_NUM; i++) {
    pinMode(RGB[i], OUTPUT);
    analogWrite(RGB[i], 100);
    delay(200);
    analogWrite(RGB[i], 0);
    delay(200);
  }
  printStatus();
}

void loop() {
  int value1 = digitalRead(BTN1);
  int value2 = digitalRead(BTN2);

  //버튼1: OFF -> ON -> 점멸
  if (value1 == 0 && prev_value1 == 1) {
    mode++;
    if (mode == 3) {
      mode = 0;
    }
    printStatus();
    delay(50);                   //버튼 떨림 방지
    value1 = digitalRead(BTN1);  //딜레이 후 상태 다시 읽기
  }
  //버튼2: LED <-> RGB
  if (value2 == 0 && prev_value2 == 1) {
    useRGB = !useRGB;
    printStatus();
    delay(50);  //버튼 떨림 방지
    value2 = digitalRead(BTN2);
  }
  prev_value1 = value1;
  prev_value2 = value2;

  switch (mode) {
    case 0:  //OFF: 설정값은 유지하고 출력만 끈다
      applyOutputs(false);
      break;
    case 1:  //ON
      updateControl();
      applyOutputs(true);
      break;
    case 2:  //점멸: ON과 같고 출력만 milli()에 따라 깜빡임
      updateControl();
      applyOutputs(milli());
      break;
  }
}

//가변저항으로 설정값 갱신 (L = 대상 선택, R = 값)
void updateControl() {
  int l = analogRead(VAR_L);
  int r = analogRead(VAR_R);
  int slot, val;

  if (useRGB) {
    slot = getSlot(l, RGB_NUM);  //3구간
    val = map(r, 0, 1023, 0, 255);
    rgbVal[slot] = val;
  } else {
    slot = getSlot(l, LED_NUM);   //4구간
    val = map(r, 0, 1023, 0, 2);  //0 / 1
    val = constrain(val, 0, 1);
    ledVal[slot] = val;
  }

  //바뀔 때만 출력
  if (!(slot == lastSlot && val == lastVal)) {
    if (useRGB) {
      Serial.print("RGB ");
    } else {
      Serial.print("LED ");
    }
    Serial.print(slot);
    Serial.print(": ");
    Serial.println(val);
    lastSlot = slot;
    lastVal = val;
  }
}

//가변저항 값(value)을 num개 구간으로 나눠서 구간 번호(0 ~ num-1) 리턴
int getSlot(int value, int num) {
  int slot = map(value, 0, 1023, 0, num);
  slot = constrain(slot, 0, num - 1);
  return slot;
}

//상태 배열을 실제 핀에 출력
void applyOutputs(boolean visible) {
  for (int i = 0; i < LED_NUM; i++) {
    if (visible && ledVal[i] == 1) {
      digitalWrite(LED[i], HIGH);
    } else {
      digitalWrite(LED[i], LOW);
    }
  }
  for (int i = 0; i < RGB_NUM; i++) {
    if (visible) {
      analogWrite(RGB[i], rgbVal[i]);
    } else {
      analogWrite(RGB[i], 0);
    }
  }
}

void printStatus() {
  Serial.print("MODE: ");
  if (mode == 0) {
    Serial.print("OFF");
  } else if (mode == 1) {
    Serial.print("ON");
  } else {
    Serial.print("BLINK");
  }
  Serial.print(" / TARGET: ");
  if (useRGB) {
    Serial.println("RGB");
  } else {
    Serial.println("LED");
  }
}

//500ms마다 토글
int milli() {
  current_time = millis();
  if (current_time - prev_time >= BLINK_MS) {
    prev_time = current_time;
    blinkState = !blinkState;
  }
  return blinkState;
}
