//13주차: 아두이노 -> p5.js 핸드셰이킹 (센서값을 받을 때마다 'B'로 다음 값 요청)
let port = new p5.WebSerial();  // 시리얼 통신을 위한 객체 생성
let connected = false;          // 아두이노의 시작신호를 받았는지

let num = 50;
let starX = [];
let starY = [];
let value1 = 0, value2 = 0;  // 가변저항1, 2 수신값 (0~1023)

let sunX = 0;
let sunY = 0;
let moonX = 0;
let moonY = 0;

let meteorX = 0;
let meteorY = 0;
let meteorOn = false;

function setup() {
  createCanvas(600, 400);
  noStroke();
  textSize(16);
  resetStars();

  port.getPorts();                       // 포트 사용 가능 여부 확인
  port.on('noport', makePortButton);     // 기존 선택한 포트가 없으면 makePortButton() 호출
  port.on('portavailable', openPort);    // 사용 가능한 포트가 있으면 openPort() 자동 호출
  port.on('data', serialEvent);          // 수신값이 있으면 serialEvent() 자동 호출
}

function draw() {
  moveSunAndMoon();
  let sunHeight = (300 - sunY) / 250;    // 태양 높이 (-1 ~ 1)
  let light = getLight(sunHeight);
  let phase = getPhase(sunHeight);

  drawSky(light);
  drawStars(light);
  drawSunAndMoon();
  drawMeteor();
  drawGround();
  drawText(phase);
  if (connected) {                       // 시작신호를 받은 뒤부터 송신
    sendData(light, phase);
  }
}

// 가변저항1: 달의 좌우 위치, 가변저항2: 절반 이상이면 달이 지평선 위(밤), 아니면 아래(낮)
function moveSunAndMoon() {
  let a = map(value1, 0, 1023, PI, 0);   // 왼쪽 끝 PI, 오른쪽 끝 0
  moonX = width / 2 + 250 * cos(a);
  if (value2 >= 512) {
    moonY = 300 - 250 * sin(a);          // 지평선 위
  } else {
    moonY = 300 + 250 * sin(a);          // 지평선 아래
  }
  sunX = width - moonX;                  // 태양은 달의 반대편
  sunY = 600 - moonY;
}

function getLight(h) {
  let light = constrain(h * 2 + 0.3, 0, 1);
  return light;
}

// 태양 높이로 시간대 구분 (0 = 낮, 1 = 노을, 2 = 밤)
function getPhase(h) {
  let phase = 2;
  if (h > 0.2) {
    phase = 0;
  } else if (h > -0.2) {
    phase = 1;
  }
  return phase;
}

// 아두이노로 송신: 동기신호, 하늘 밝기(0~255), 시간대(0~2)
function sendData(light, phase) {
  port.write('A');                       // 동기신호 송신
  port.write(int(light * 255));          // 하늘 밝기 송신
  port.write(phase);                     // 시간대 송신
}

function drawSky(light) {
  let skyColor = color(20 + light * 100, 20 + light * 150, 50 + light * 200);
  background(skyColor);
}

function resetStars() {
  for (let i = 0; i < num; i++) {
    starX[i] = random(width);
    starY[i] = random(280);
  }
}

function drawStars(light) {
  if (light < 0.5) {
    let twinkle = 150 + 100 * sin(millis() * 0.005);
    stroke(twinkle);
    strokeWeight(3);
    for (let i = 0; i < num; i++) {
      point(starX[i], starY[i]);
    }
    noStroke();
    strokeWeight(1);
  }
}

function drawSunAndMoon() {
  fill(255, 200, 50);
  ellipse(sunX, sunY, 50, 50);
  fill(240, 240, 210);
  ellipse(moonX, moonY, 40, 40);
}

function launchMeteor(x, y) {
  meteorX = x;
  meteorY = y;
  meteorOn = true;
}

function drawMeteor() {
  if (meteorOn) {
    stroke(255);
    line(meteorX, meteorY, meteorX - 40, meteorY - 20);
    noStroke();
    meteorX += 8;
    meteorY += 4;
    if (meteorX > width + 40) {
      meteorOn = false;
    }
  }
}

function drawGround() {
  fill(30, 90, 40);
  rect(0, 300, width, 100);
  fill(50, 130, 60);
  arc(width / 2, 400, width * 1.5, 200, PI, TWO_PI);
}

function drawText(phase) {
  fill(255);
  if (phase == 0) {
    text("낮", 20, 30);
  } else if (phase == 1) {
    text("노을", 20, 30);
  } else {
    text("밤", 20, 30);
  }

  if (dist(mouseX, mouseY, moonX, moonY) < 20) {
    text("달", mouseX + 10, mouseY);
  }
}

function mousePressed() {
  launchMeteor(mouseX, mouseY);
}

function keyPressed() {
  switch (key) {
    case "r":
      resetStars();
      break;
    case "m":
      launchMeteor(random(width), random(100));
      break;
  }
}

// 아두이노에서 수신 (13주차): 시작신호 "A" 또는 "가변저항1,가변저항2,pressed"
function serialEvent() {
  let str = port.readLine();             // 개행까지의 수신 문자열 읽기
  if (!str) return;                      // 입력 문자열이 없으면 함수 빠져나가기
  let s = str.trim();                    // 시작과 끝에 붙은 공백 삭제 (p5.js 2.x는 trim() 없음)
  let value = s.split(",");              // 콤마로 구분된 문자열을 value 배열에 저장 (2.x는 split() 없음)
  port.write('B');                       // 송신: 다음 센서값 요청 (핸드셰이킹)
  connected = true;                      // 한 줄이라도 받았으니 통신 시작
  if (value.length != 3) return;         // 시작신호('A')인 경우 함수 벗어나기
  value1 = float(value[0]);              // 가변저항1 (실수로 변환)
  value2 = float(value[1]);              // 가변저항2
  if (value[2] === "pressed") {          // 버튼을 누른 순간
    launchMeteor(random(width), random(100));
  }
}

function makePortButton() {              // 포트 첫 연결 시 포트 선택 버튼 생성
  let portButton = createButton('포트 선택');
  portButton.mousePressed(choosePort);   // 버튼 누르면 choosePort() 자동 호출
  function choosePort() {                // 콜백함수
    port.requestPort();                  // 시리얼 포트 선택창 오픈
  }
}

function openPort() {                    // 콜백함수
  port.open().then(initiateSerial);      // 포트를 오픈 후 initiateSerial() 자동 호출
  function initiateSerial() {            // 콜백함수
    console.log('포트 오픈');
  }
}
