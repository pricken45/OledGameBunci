/**
 * @file main.cpp
 * @author Ruben Stenlund (rubo@rubo.se)
 * @brief
 * @version 1.0
 * @date 2022-11-28
 *
 * @copyright Copyright (c) 2022
 *
 */

#include <Enemy.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

float x;
float y;

float lastX = x;
float lastY = y;

const float gravity = 0.15;
float yVelocity = 0;
float xVelocity = 0;

const float jumpForce = 2.5;
const float drag = 0.96;
bool menu = true;

int lastButtonState = 0;
bool button_debounce = false;
bool button_debounceR = false;
bool button_debounceReset = false;

bool startGame = false;

int score = 0;
int scoreAdder = 0;

int alive = 0;
int aliveAddder = 0;

int highScore = 0;

// Enemy enemies[15] = {Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false), Enemy(display, 3, false)};
Enemy e1 = Enemy(&display, 1, true);
Enemy e2 = Enemy(&display, 1, true);
Enemy e3 = Enemy(&display, 1, true);
Enemy e4 = Enemy(&display, 1, false);
Enemy e5 = Enemy(&display, 1, false);

void clearEEPROM()
{

  // to clear eeprom, and clear the highscore
  for (unsigned i = 0; i < EEPROM.length(); i++)
  {
    EEPROM.write(i, 0);
  }
}

void resetPosition()
{
  // reset the players position and make variables ready for game restart

  if (score > highScore)
  {
    EEPROM.put(0, score);
    highScore = score;
  }

  x = SCREEN_WIDTH / 2;
  y = 5;
  yVelocity = 0;
  xVelocity = 0;
  score = 0;
  e1.newPos();
  e2.newPos();
  e3.newPos();
  e4.newPos();
  e5.newPos();

  e1.addition = 0;
  e2.addition = 0;
  e3.addition = 0;
  e4.addition = 0;
  e5.addition = 0;

  e4.alive = false;
  e5.alive = false;
}

void drawCentreString(const char *buf, int x, int y)
{
  // function for drawing a string at the center
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(buf, x, y, &x1, &y1, &w, &h); // calc width of new string
  display.setCursor((x - w) / 2, y);
  display.print(buf);
}
void death()
{
  // play death sound effect

  startGame = false;
  tone(A2, 2000, 500);
  delay(500);
  tone(A2, 400, 500);
}

void startMenu()
{
  // shows the start menu and waits for player input to start the game

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.clearDisplay();

  while (!startGame)
  {
    display.drawRect(16, 23, 41, 15, WHITE);
    drawCentreString("Press any", SCREEN_WIDTH, 13);
    drawCentreString("button to begin", SCREEN_WIDTH, 27);
    String highScoreS = "Highscore: ";
    highScoreS += highScore;
    drawCentreString(highScoreS.c_str(), SCREEN_WIDTH, 50);
    display.display();
    if ((!digitalRead(2)) == LOW && button_debounce == false)
    {
      startGame = true;
      button_debounce = true; // toggle button debounce flag
    }

    if ((!digitalRead(2)) == HIGH && button_debounce == true)
    { // this will prevent subsequent presses
      button_debounce = false;
    }

    if ((!digitalRead(3)) == LOW && button_debounceR == false)
    {
      startGame = true;
      button_debounceR = true; // toggle button debounce flag
    }

    if ((!digitalRead(3)) == HIGH && button_debounceR == true)
    { // this will prevent subsequent presses
      button_debounceR = false;
    }
    if ((!digitalRead(4)) == LOW && button_debounceReset == false)
    {
      death();
      clearEEPROM();
      highScore = 0;
      button_debounceReset = true; // toggle button debounce flag
    }

    if ((!digitalRead(4)) == HIGH && button_debounceReset == true)
    { // this will prevent subsequent presses
      button_debounceReset = false;
    }
  }

  // rendering the game start count down

  display.clearDisplay();

  display.setCursor(64, 25);
  display.println("3");
  display.display();
  tone(A2, 800, 300);

  delay(1000);
  display.clearDisplay();
  display.setCursor(64, 25);
  display.println("2");
  display.display();
  tone(A2, 800, 300);
  delay(1000);
  display.clearDisplay();
  display.setCursor(64, 25);
  display.println("1");
  display.display();
  tone(A2, 1500, 500);

  delay(1000);
  menu = false;
  resetPosition();
  scoreAdder = 0;
}

void collisionDetection()
{
  // function for detecting player collision with walls

  if (y >= SCREEN_HEIGHT || y <= -2 || x <= 0 || x >= SCREEN_WIDTH)
  {
    resetPosition();
    death();
    startMenu();
  }
}

void clickLeft()
{
  // click handler for left button

  yVelocity = -jumpForce;
  xVelocity = -2;
  tone(A2, 2000, 70);
}
void clickRight()
{
  // click handler for right button

  yVelocity = -jumpForce;
  xVelocity = 2;
  tone(A2, 2000, 70);
}

void setup()
{
  // setup function, runs once

  pinMode(2, INPUT);
  pinMode(3, INPUT);
  pinMode(4, INPUT);

  int recv;
  EEPROM.get(0, recv);

  if (recv != 821)
  {
    EEPROM.get(0, highScore);
  }
  else
  {
    EEPROM.put(0, 0);
  }

  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }

  startMenu();
}

void loop()
{
  // loop function. runs over and over. all logic happens here or get called here

  if (menu)
  {
    return;
  }

  // "physics" simulation

  yVelocity += gravity;
  xVelocity *= drag;

  y += yVelocity;
  x += xVelocity;

  // handling the score adder

  scoreAdder++;

  if (scoreAdder > 40)
  {
    scoreAdder = 0;
    score++;
  }

  if (score >= 10)
  {
    e4.alive = true;
    e5.alive = true;
  }

  // Serial.println(enemy.x);

  collisionDetection();

  // jumping click registrations

  if ((!digitalRead(2)) == LOW && button_debounce == false)
  {
    startGame = true;
    clickLeft();
    button_debounce = true; // toggle button debounce flag
  }

  if ((!digitalRead(2)) == HIGH && button_debounce == true)
  { // this will prevent subsequent presses
    button_debounce = false;
  }

  if ((!digitalRead(3)) == LOW && button_debounceR == false)
  {
    startGame = true;
    clickRight();
    button_debounceR = true; // toggle button debounce flag
  }

  if ((!digitalRead(3)) == HIGH && button_debounceR == true)
  { // this will prevent subsequent presses
    button_debounceR = false;
  }

  // Rendering all nessecary items for playing the game.
  display.clearDisplay();
  e1.update();
  e2.update();
  e3.update();
  e4.update();
  e5.update();

  e1.render();
  e2.render();
  e3.render();
  e4.render();
  e5.render();

  if (e1.colliding(x, y, lastX, lastY))
  {
    resetPosition();
    death();
    startMenu();
  }
  if (e2.colliding(x, y, lastX, lastY))
  {
    resetPosition();
    death();
    startMenu();
  }
  if (e3.colliding(x, y, lastX, lastY))
  {
    resetPosition();
    death();
    startMenu();
  }
  if (e5.colliding(x, y, lastX, lastY))
  {
    resetPosition();
    death();
    startMenu();
  }
  if (e5.colliding(x, y, lastX, lastY))
  {
    resetPosition();
    death();
    startMenu();
  }

  display.setTextColor(WHITE);
  display.setCursor(61, 3);
  display.setTextSize(1.5);
  display.println(score);

  display.drawPixel(x, y, WHITE);
  delay(15);
  display.display();

  lastButtonState = digitalRead(2);

  lastX = x;
  lastY = y;

  // Serial.println(digitalRead(2));
}