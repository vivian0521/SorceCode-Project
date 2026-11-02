//10주차
#define BTN1 4
#define BTN2 2
#define VAR_L A1
#define VAR_R A2
//핀 번호 지정
const int LED[] = { 12, 8, 7, 6 };  //0번 비트 = LED[0](12번 핀) ... 3번 비트 = LED[3](6번 핀)
const int RGB[] = { 11, 5, 3 };     //Red 11, Green 5, Blue 3

#define LED_NUM 4
#define RGB_NUM 3
#define BLINK_MS 500
#define POT_MOVE 20  //R 가변저항을 이만큼 이상 돌려야 값이 바뀌기 시작함

//state (설정값은 OFF 모드에서도 유지됨)
byte ledBits = 0b0000;        //LED 4개 상태를 비트 하나씩 저장 (1 = 켜짐, 0 = 꺼짐)
int rgbVal[3] = { 0, 0, 0 };  //0 ~ 255

int mode = 0;             //버튼1: 0 = OFF, 1 = ON, 2 = 점멸
boolean useRGB = false;   //버튼2: false = LED, true = RGB
boolean usePot = true;    //true = 가변저항 모드, false = 시리얼 모드
int prevR = 0;            //R 가변저항 기준 위치 (0 ~ 1023)
boolean potGrab = false;  //R 가변저항을 기준 위치에서 충분히 돌렸는지
int prev_value1 = 1, prev_value2 = 1;
int lastSlot = -1;
boolean blinkState = true;
unsigned long current_time, prev_time;

void setup() {
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
  pinMode(VAR_L, INPUT);
  pinMode(VAR_R, INPUT);
  Serial.begin(9600);
  Serial.setTimeout(50);  //readStringUntil(), parseInt()가 기다리는 최대 시간 (기본 1000ms)

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
  resetPotBase();
  Serial.println("Commands: led 1010 / led set|clr|tog|chk N / led tog / rgb R,G,B");
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
    resetPotBase();  //OFF 동안 돌린 손잡이는 무시
    printStatus();
    delay(50);                   //버튼 떨림 방지
    value1 = digitalRead(BTN1);  //딜레이 후 상태 다시 읽기
  }
  //버튼2: LED <-> RGB, 가변저항 모드로 복귀
  if (value2 == 0 && prev_value2 == 1) {
    useRGB = !useRGB;
    usePot = true;
    lastSlot = -1;   //선택 칸 다시 출력
    resetPotBase();  //지금 손잡이 위치를 새 기준으로
    printStatus();
    delay(50);  //버튼 떨림 방지
    value2 = digitalRead(BTN2);
  }
  prev_value1 = value1;
  prev_value2 = value2;

  readSerial();  //키보드 입력 확인

  switch (mode) {
    case 0:  //OFF: 설정값은 유지하고 출력만 끈다
      applyOutputs(false);
      break;
    case 1:  //ON
      if (usePot) {
        updateControl();
      }
      applyOutputs(true);
      break;
    case 2:  //점멸: ON과 같고 출력만 milli()에 따라 깜빡임
      if (usePot) {
        updateControl();
      }
      applyOutputs(milli());
      break;
  }
}

//가변저항으로 설정값 갱신 (L = 대상 선택, R = 값)
void updateControl() {
  int l = analogRead(VAR_L);
  int r = analogRead(VAR_R);
  int slot;

  if (useRGB) {
    slot = getSlot(l, RGB_NUM);  //3구간
  } else {
    slot = getSlot(l, LED_NUM);  //4구간
  }

  //L: 칸 선택만 한다 (지나가는 칸의 값은 바꾸지 않음)
  if (slot != lastSlot) {
    if (useRGB) {
      Serial.print("SELECT RGB ");
    } else {
      Serial.print("SELECT LED ");
    }
    Serial.println(slot);
    if (lastSlot != -1) {
      resetPotBase();  //칸을 바꾸면 R을 다시 돌려야 값이 바뀜
    }
    lastSlot = slot;
  }

  //R: 돌렸을 때만 선택된 칸에 값 반영
  if (!potRMoved(r)) {
    return;
  }

  if (useRGB) {
    int val = map(r, 0, 1023, 0, 255);
    if (val != rgbVal[slot]) {
      rgbVal[slot] = val;
      Serial.print("RGB ");
      Serial.print(slot);
      Serial.print(": ");
      Serial.println(val);
    }
  } else {
    byte before = ledBits;
    if (r >= 512) {
      ledBits |= (1 << slot);  //비트 세트
    } else {
      ledBits &= ~(1 << slot);  //비트 리셋
    }
    if (ledBits != before) {
      printLedBits();
    }
  }
}

//R 가변저항이 기준 위치에서 POT_MOVE 이상 움직였는지 확인
//한 번 넘으면(potGrab) 그 뒤로는 돌리는 대로 따라감
boolean potRMoved(int r) {
  if (!potGrab) {
    if (abs(r - prevR) < POT_MOVE) {
      return false;  //아직 덜 돌림 -> 값 유지
    }
    potGrab = true;
  }
  prevR = r;
  return true;
}

//지금 R 손잡이 위치를 새 기준으로 저장하고, 다시 돌릴 때까지 값 반영을 멈춤
void resetPotBase() {
  prevR = analogRead(VAR_R);
  potGrab = false;
}

//가변저항 값(value)을 num개 구간으로 나눠서 구간 번호(0 ~ num-1) 리턴
int getSlot(int value, int num) {
  int slot = map(value, 0, 1023, 0, num);
  slot = constrain(slot, 0, num - 1);
  return slot;
}

//키보드 입력 처리
//led 1010 / led set 2 / led clr 2 / led tog 2 / led tog / led chk 2 / rgb 113,205,100
void readSerial() {
  if (!Serial.available()) {
    return;
  }
  String cmd = Serial.readStringUntil(' ');  //첫 단어 (led / rgb)
  cmd.trim();

  if (cmd.equals("rgb")) {
    int r = Serial.parseInt();  //쉼표 같은 숫자가 아닌 문자는 건너뛰고 정수만 읽음
    int g = Serial.parseInt();
    int b = Serial.parseInt();
    Serial.readStringUntil('\n');  //줄 끝에 남은 문자 비우기
    rgbVal[0] = constrain(r, 0, 255);
    rgbVal[1] = constrain(g, 0, 255);
    rgbVal[2] = constrain(b, 0, 255);
    Serial.print("RGB: ");
    Serial.print(rgbVal[0]);
    Serial.print(", ");
    Serial.print(rgbVal[1]);
    Serial.print(", ");
    Serial.println(rgbVal[2]);
    toSerialMode();
  } else if (cmd.equals("led")) {
    String arg = Serial.readStringUntil('\n');  //나머지 (1010 / set 2 / tog ...)
    arg.trim();
    ledCommand(arg);
  } else {
    if (Serial.available()) {
      Serial.readStringUntil('\n');  //남은 문자 비우기
    }
    Serial.println("Unknown command");
  }
}

//led 뒤에 오는 명령 처리 (비트 세트/리셋/검사/반전)
void ledCommand(String arg) {
  //led 1010: 4자리 패턴 (왼쪽 = 3번 비트, 오른쪽 = 0번 비트)
  if (isPattern(arg)) {
    for (int i = 0; i < LED_NUM; i++) {
      int bit = 3 - i;
      if (arg.charAt(i) == '1') {
        ledBits |= (1 << bit);  //세트
      } else {
        ledBits &= ~(1 << bit);  //리셋
      }
    }
    printLedBits();
    toSerialMode();
    return;
  }

  //led tog: 4개 모두 반전
  if (arg.equals("tog")) {
    ledBits ^= 0b1111;
    printLedBits();
    toSerialMode();
    return;
  }

  //led set|clr|tog|chk N
  if (arg.length() != 5 || arg.charAt(3) != ' ') {
    Serial.println("Wrong LED command");
    return;
  }
  String op = arg.substring(0, 3);
  char c = arg.charAt(4);
  if (c < '0' || c > '3') {
    Serial.println("LED number: 0 ~ 3");
    return;
  }
  int n = c - '0';  //문자 '2' -> 정수 2

  if (op.equals("set")) {
    ledBits |= (1 << n);  //세트: n번 비트만 1로
  } else if (op.equals("clr")) {
    ledBits &= ~(1 << n);  //리셋: n번 비트만 0으로
  } else if (op.equals("tog")) {
    ledBits ^= (1 << n);  //반전: n번 비트만 뒤집기
  } else if (op.equals("chk")) {
    Serial.print("LED ");  //검사: 값은 안 바꾸므로 가변저항 모드 유지
    Serial.print(n);
    if (ledBits & (1 << n)) {
      Serial.println(": ON");
    } else {
      Serial.println(": OFF");
    }
    return;
  } else {
    Serial.println("Wrong LED command");
    return;
  }
  printLedBits();
  toSerialMode();
}

//4글자이고 모두 0 또는 1이면 true
boolean isPattern(String s) {
  if (s.length() != LED_NUM) {
    return false;
  }
  for (int i = 0; i < LED_NUM; i++) {
    if (s.charAt(i) != '0' && s.charAt(i) != '1') {
      return false;
    }
  }
  return true;
}

//시리얼로 값을 넣으면 가변저항 입력을 끔
void toSerialMode() {
  if (usePot) {
    usePot = false;
    Serial.println("INPUT: SERIAL (press BTN2 to use knobs)");
  }
}

//상태 배열/비트를 실제 핀에 출력
void applyOutputs(boolean visible) {
  for (int i = 0; i < LED_NUM; i++) {
    if (visible && (ledBits & (1 << i))) {  //비트 검사
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

//LED 비트를 항상 4자리로 출력 (BIN은 앞자리 0을 생략하므로 직접 검사)
void printLedBits() {
  Serial.print("LED: ");
  for (int i = LED_NUM - 1; i >= 0; i--) {
    if (ledBits & (1 << i)) {
      Serial.print('1');
    } else {
      Serial.print('0');
    }
  }
  Serial.print(" (BIN ");
  Serial.print(ledBits, BIN);
  Serial.println(")");
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
    Serial.print("RGB");
  } else {
    Serial.print("LED");
  }
  Serial.print(" / INPUT: ");
  if (usePot) {
    Serial.println("KNOB");
  } else {
    Serial.println("SERIAL");
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
