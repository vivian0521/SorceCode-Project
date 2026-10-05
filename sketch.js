//6주차
#define BTN1 2
#define BTN2 4
#define Red 11
#define Green 5
#define Blue 3
boolean blink = false;
unsigned long current_time, prev_time;

void setup() {
  pinMode(BTN1, INPUT);
  pinMode(BTN2, INPUT);

  pinMode(Red, OUTPUT);
  pinMode(Green, OUTPUT);
  pinMode(Blue, OUTPUT);
  Serial.begin(9600);

  for(int i=0;i<3;i++){
    digitalWrite(Red, 1);
    delay(200);
    digitalWrite(Red, 0);
    delay(300);
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
      digitalWrite(Red, blink);
      digitalWrite(Green,0);
      digitalWrite(Blue,0);
      break;
    case 2:
      digitalWrite(Red, 0);
      digitalWrite(Green, blink);
      digitalWrite(Blue,0);
      break;
    case 3:
      digitalWrite(Red, 0);
      digitalWrite(Green,0);
      digitalWrite(Blue,blink);
      break;
    default:
      digitalWrite(Red, 0);
      digitalWrite(Green,0);
      digitalWrite(Blue,0);
      break;
  }
}
