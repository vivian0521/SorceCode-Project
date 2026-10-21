//7주차 v.1
#define BTN1 2
#define BTN2 4
#define VARIABLE_R A1 //이거 방향 확인 해야함
#define VARIABLE_L A2 //동일
//#define Red 11, Green 5, Blue 3

const int LED = {6, 7, 8, 9};
const int RGB = {11, 5, 3};

boolean blink = false;
unsigned long current_time, prev_time;

void setup() {
//Button 2개 INPUT
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);
//가변저항 2개 INPUT
  pinMode(VARIABLE_R, INPUT);
  pinMode(VARIABLE_L, INPUT);
//RGB OUTPUT
  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Blue, OUTPUT);

//배열->LED 4개 OUTPUT
  for(int i = 0; i<4; i++){
    pinMode(LED[i], OUTPUT);
    digitalWrite(LED[i], HIGH);
    delay(100);
    digitalWrite(LED[i], LOW);
    delay(100);
  }
  Serial.begin(9600);

//RGB 깜빡 3번
  for(int i=0;i<3;i++){
    analogWrite(RGB[i], 150);//밝기 150 고정(: 눈 아픔) (~255)
    digitalWrite(RGB[i], 1);
    delay(200);
    digitalWrite(RGB[i], 0);
    delay(200);
  }
}

void loop() {
  current_time = millis();
  if(current_time - prev_time >= 500){
    prev_time = current_time;
    blink = !blink;
  }


  int state = 0;
  if(digitalRead(BTN1)==LOW && digitalRead(BTN2)==LOW){
    state = 3;
  } else if(digitalRead(BTN1) == LOW){
    state = 1;
  } else if(digitalRead(BTN2)==LOW){
    state = 2;
  }
    else state = 0;

  switch(state){
    case 1:
      digitalWrite(RED[1], blink);
      digitalWrite(RED[1],0);
      digitalWrite(RED[1],0);
      break;
    case 2:
      digitalWrite(RED[1], 0);
      digitalWrite(RED[1], blink);
      digitalWrite(RED[1],0);
      break;
    case 3:
      digitalWrite(RED[1], 0);
      digitalWrite(RED[1],0);
      digitalWrite(RED[1],blink);
      break;
    default:
      digitalWrite(RED[1], 0);
      digitalWrite(RED[1],0);
      digitalWrite(RED[1],0);
      break;
  }
}
