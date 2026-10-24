//주제: 지구에서 본 달의 공전 활동
//아침엔 해, 밤에는 달이 뜨는 하루를 표현함. 가끔 별똥별도 떨어집니다.
//학번: 2024111477, 이름: 김태영
let num = 50;
let starX = [];
let starY = [];
let theta = 0;

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
}

function draw() {
  updateTime();
  let sunHeight = sin(theta);
  let light = getLight(sunHeight);

  drawSky(light);
  drawStars(light);
  drawSunAndMoon();
  drawMeteor();
  drawGround();
  drawText(sunHeight);
}

function updateTime() {
  if (mouseIsPressed) {
    theta += 0.03;
  } else {
    theta += 0.005;
  }
}

function getLight(h) {
  let light = constrain(h * 2 + 0.3, 0, 1);
  return light;
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
  let sunX = width / 2 - 250 * cos(theta);
  let sunY = 300 - 250 * sin(theta);
  moonX = width / 2 + 250 * cos(theta);
  moonY = 300 + 250 * sin(theta);

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

function drawText(h) {
  fill(255);
  if (h > 0.2) {
    text("낮", 20, 30);
  } else if (h > -0.2) {
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
    case 'r':
      resetStars();
      break;
    case 'm':
      launchMeteor(random(width), random(100));
      break;
  }
}
