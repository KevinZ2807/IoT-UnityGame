#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128  // OLED width,  in pixels
#define SCREEN_HEIGHT 64  // OLED height, in pixels

int state = LOW;

int RED_SIGNAL = 5;
int GREEN_SIGNAL = 4;
int BLUE_SIGNAL = 2;

// Buttons
const int rightButton = 34;
const int leftButton = 35;
const int changeModeButton = 32;

int rightButtonState = LOW;
int leftButtonState = LOW;
int changeModeButtonState = LOW;

// OLED AND MPU
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_MPU6050 mpu;

float giroX = 0;
float giroY = 0;
float giroZ = 0;
int countdown = 0;

bool startLeft = false;
bool startRight = false;
bool startJump = false;
bool startActive = false;

void resetValues(bool &start, int &countdown) {
    start = false;
    countdown = 0;
}

int mode = 0;  // Accelerometer mode = 0; Button mode = 1
void setup(void) {
  Serial.begin(115200);
  mode = 0;

  pinMode(RED_SIGNAL, OUTPUT);
  pinMode(GREEN_SIGNAL, OUTPUT);
  pinMode(BLUE_SIGNAL, OUTPUT);

  pinMode(rightButton, INPUT);
  pinMode(leftButton, INPUT);
  pinMode(changeModeButton, INPUT);
  pinMode(speech, INPUT);

  // SET LED LIGHT TO RED WHEN NOT READY
  analogWrite(RED_SIGNAL, 255);
  analogWrite(GREEN_SIGNAL, 0);
  analogWrite(BLUE_SIGNAL, 0);

  // START
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
      ;
  }

  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("failed to start SSD1306 OLED"));
    while (1)
      ;
  }
  delay(2000);          // wait two seconds for initializing
  oled.clearDisplay();  // clear display

  oled.setTextSize(1);         // set text size
  oled.setTextColor(WHITE);    // set text color
  oled.setCursor(0, 2);        // set position to display (x,y)
  oled.println("Robotronix");  // set text
  oled.display();              // display on OLED

  analogWrite(RED_SIGNAL, 0);
  analogWrite(GREEN_SIGNAL, 255);  // SET COLOR TO GREEN WHEN READY
  analogWrite(BLUE_SIGNAL, 0);
}

void loop() {
  changeModeButtonState = digitalRead(changeModeButton);
  state = digitalRead(speech);

  if (state == HIGH) {
    mode = 1 - mode;
  }

  if (changeModeButtonState == HIGH) {
    while (digitalRead(changeModeButton) == HIGH)
      ;
    mode = 1 - mode;  // Change from 1 to 0 and 0 to 1
  }

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  oled.clearDisplay();
  oled.setTextSize(1);
  oled.setTextColor(WHITE, 0);
  oled.setCursor(0, 0);

  oled.println("Acelerometro:");
  oled.print(a.acceleration.x);
  oled.print(",");
  oled.print(a.acceleration.y);
  oled.print(",");
  oled.println(a.acceleration.z);
  oled.println("");

  oled.println("Giroscopio:");
  oled.print(g.gyro.x);
  oled.print(",");
  oled.print(g.gyro.y);
  oled.print(",");
  oled.print(g.gyro.z);
  oled.println();

  if (mode == 0) {
    oled.println("Mode: Accelerometer");
    analogWrite(RED_SIGNAL, 0);
    analogWrite(GREEN_SIGNAL, 255);
    analogWrite(BLUE_SIGNAL, 0);
  }
  if (mode == 1) {
    oled.println("Mode: Buttons");
    analogWrite(RED_SIGNAL, 0);
    analogWrite(GREEN_SIGNAL, 0);
    analogWrite(BLUE_SIGNAL, 255);
  }
  oled.display();


  //Serial.print(map(a.acceleration.z, -10.00, 10.00, -3000.00, 3000.00));
  Serial.print(a.acceleration.z);
  Serial.print(",");
  /* Get new sensor events with the readings */
  if (mode == 0) {
    Serial.print(a.acceleration.y);
    Serial.print(",");
    // Serial.print(g.gyro.x);
    // Serial.print(",");
    // Serial.print(g.gyro.y);
    // Serial.print(",");
    // Serial.println(g.gyro.z);
    if (g.gyro.x <= -4.0f || g.gyro.z >= 4.0f && startRight == false) { // Bat dau dash trai
      startLeft = true;
    }
    
    if (startLeft) { // Neu dang dash trai
      countdown++;
      if (g.gyro.x >= 4.0f || g.gyro.z <= -4.0f) {
        startActive = true;
        Serial.println("DASHLEFT");
        resetValues(startLeft, countdown);
      }
    }

    if (g.gyro.x >= 4.0f || g.gyro.z <= -4.0f && startLeft == false) { // Bat dau dash phai
      startRight = true;
    }
    if (startRight) { // Neu dang dash phai
      countdown++;
      if (g.gyro.x <= -4.0f || g.gyro.z >= 4.0f) {
          startActive = true;
          Serial.println("DASHRIGHT");
          resetValues(startRight, countdown);
      }
    }

    if (g.gyro.y <= -4.0f) { // Bat dau nhay
      startJump = true;
    }
    if (startJump) { // Neu dang nhay
      countdown++;
      if (g.gyro.y >= 4.0f) {
          startActive = true;
          Serial.println("JUMP");
          resetValues(startJump, countdown);
      }
    }

    if (countdown >= 5) {
      resetValues(startLeft, countdown);
      resetValues(startRight, countdown);
      resetValues(startJump, countdown);
    }
    if (!startActive) {
      Serial.println("NONE");
    }
    startActive = false;
  }

  if (mode == 1) {
    rightButtonState = digitalRead(rightButton);
    leftButtonState = digitalRead(leftButton);
    if (rightButtonState == HIGH) {
      Serial.print("10");
      oled.println("RIGHT");
      oled.display();
    } else if (leftButtonState == HIGH) {
      Serial.print("-10");
      oled.println("LEFT");
      oled.display();
    } else {
      Serial.print("0");
    }
    Serial.print(",");
    Serial.println("NONE");
  }
  delay(10);
}