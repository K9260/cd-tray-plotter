#include <Stepper.h>
#include <Servo.h>

#define REVERSE_X      1
#define REVERSE_Y      0
#define STEPS_PER_REV  20
#define MAX_STEPS      250
#define PEN_UP_ANGLE   50
#define PEN_DOWN_ANGLE 80
#define SERVO_DELAY   (PEN_DOWN_ANGLE - PEN_UP_ANGLE) * 4
#define LETTER_SPACING 2
#define SERVO_PIN      10
#define PIXEL_SIZE     6
#define SYMBOL_HEIGHT  7
#define SYMBOL_WIDTH   8

Stepper X_MOTOR(STEPS_PER_REV, 6, 7, 8, 9);
Stepper Y_MOTOR(STEPS_PER_REV, 2, 3, 4, 5);
Servo servo;

struct point {
  uint8_t x, y, z;
  /*
      Directly initialize axis rather than assign new value after initialization.
      Constructor member initialization on Google.
  */
  point() : x(0), y(0), z(0) {}
  point(uint8_t _x, uint8_t _y) : x(_x), y(_y), z(0) {}
  point(uint8_t _x, uint8_t _y, uint8_t _z) : x(_x), y(_y), z(_z) {}

  point operator + (const point& p) {
    return point(this->x + p.x, this->y + p.y);
  }
  point operator - (const point& p) {
    return point(this->x - p.x, this->y - p.y);
  }
  bool operator == (const point& p) {
    return (this->x == p.x && this->y == p.y && this->z == p.z);
  }
  bool operator != (const point& p) {
    return (this->x != p.x || this->y != p.y || this->z != p.z);
  }
};

struct point rectangle[] = {
  {0, MAX_STEPS, 0},
  {MAX_STEPS, MAX_STEPS, 0},
  {MAX_STEPS , 0, 0},
  {0, 0, 0}
};

struct point shape[] = {
  {0, MAX_STEPS / 2, 0},
  {MAX_STEPS / 2, MAX_STEPS, 0},
  {MAX_STEPS, MAX_STEPS / 2, 0},
  {MAX_STEPS / 2, 0, 0}
};

struct point triangle[] = {
  {0, 0, 0},
  {MAX_STEPS, 0, 0},
  {0, MAX_STEPS, 0},
};

struct point line[] = {
  {0, 0, 0},
  {118, 213, 0}
};

struct point location = {0, 0, 0};

char _R[] = {
  0B11110000,
  0B10010000,
  0B10010000,
  0B11110000,
  0B11000000,
  0B10100000,
  0B10010000,
};
char _E[] = {
  0B11110000,
  0B10010000,
  0B10000000,
  0B11100000,
  0B10000000,
  0B10010000,
  0B11110000,
};
char _K[] = {
  0B10010000,
  0B10100000,
  0B11000000,
  0B10000000,
  0B11000000,
  0B10100000,
  0B10010000,
};
char _O[] = {
  0B11110000,
  0B10010000,
  0B10010000,
  0B10010000,
  0B10010000,
  0B10010000,
  0B11110000,
};

void setup() {
  Serial.begin(9600);
  X_MOTOR.setSpeed(1000);
  Y_MOTOR.setSpeed(1000);
  servo.attach(SERVO_PIN);
  penUp();
}

void loop() {
  String str;
  struct point node;
  if (Serial.available()) {
    str = Serial.readString();
    int32_t coords = str.toInt();
    if (coords == -1) {
      releaseMotors();
    }/* else {
      node.x = coords >> 16 & 0xFF;
      node.y = coords >> 8 & 0xFF;
      node.z = coords >> 0 & 0xFF;
    }*/
    if (coords == 10) {
      drawSymbol(_R, PIXEL_SIZE, 'h');
      drawSymbol(_E, PIXEL_SIZE, 'h');
      drawSymbol(_K, PIXEL_SIZE, 'h');
      drawSymbol(_O, PIXEL_SIZE, 'h');
      releaseMotors();
    }
    if (coords == 11) {
      //drawCircle({MAX_STEPS/2, MAX_STEPS/2}, 50);
      drawShape(rectangle, 4);
      drawShape(shape, 4);
      releaseMotors();
    }
  }

  if (node.x || node.y) {
    drawVector(node);
  }
  delay(1000);
}

void drawShape (struct point nodes[], uint8_t len) {
  moveTo(nodes[0]);
  for (uint8_t i = 0; i < len + 1; i++) {
    drawVector(nodes[i % len]);
  }
  moveTo({0, 0});
  releaseMotors();
}

void drawVector(struct point node) {
  uint8_t xDistance = abs(node.x - location.x);          //Distance to travel on x-axis
  uint8_t yDistance = abs(node.y - location.y);          //Distance to travel on y-axis
  char longerAxis = xDistance > yDistance ? 'x' : 'y';   //Which axis has longer distance to travel
  float scale = (longerAxis == 'x') ? (float)(xDistance) / (float)(yDistance) : (float)(yDistance) / (float)(xDistance);

  while (location != node) { //Loop until motors reach target node
    moveZ(node.z);                                       //Move the pen up/down
    if (scale < MAX_STEPS) {                             //If scale is equal to max steps, there is no need to limit other axis's
      switch (longerAxis) {
        case 'x':                                        //x-axis travels freely, y-axis is limited
          moveX(node.x);
          if (yDistance * scale >= xDistance) {
            moveY(node.y);
          }
          break;
        case 'y':                                       //y-axis travels freely, x-axis is limited
          moveY(node.y);
          if (xDistance * scale >= yDistance) {
            moveX(node.x);
          }
          break;
      }
    } else {
      moveX(node.x);
      moveY(node.y);
    }
    xDistance = abs(node.x - location.x);               //Recalculate the distance for next loop
    yDistance = abs(node.y - location.y);
  }
}

/*
  Iterates through a whole column, then increases row and goes again
*/

void drawSymbol(char arr[SYMBOL_HEIGHT], uint8_t pixelSize, char fillType) {
  static uint8_t offsetY = 0; //If space is running out on X axis, move one row down
  if (location.x + LETTER_SPACING * pixelSize + SYMBOL_WIDTH * pixelSize >= MAX_STEPS) {
    moveTo({0, location.y + pixelSize * LETTER_SPACING});
    offsetY = location.y;
  }
  uint8_t offsetX = location.x + pixelSize * LETTER_SPACING;
  struct point pixel;
  for (uint8_t x = 0; x < SYMBOL_WIDTH; x++) {
    for (uint8_t y = 0; y < SYMBOL_HEIGHT; y++) {
      if (arr[y] >> SYMBOL_WIDTH - 1 - x & 1) {
        pixel = {pixelSize * x + offsetX, pixelSize * y + offsetY};
        switch (fillType) {
          case 'v':
            verticalFill(pixel, pixelSize);
            break;
          case 'h':
            horizontalFill(pixel, pixelSize);
            break;
          default:
            horizontalFill(pixel, pixelSize);
        }
      }
    }
  }
}

void horizontalFill(struct point start, uint8_t pixelSize) {
  uint8_t i = start.y;
  if (location != start) {
    moveTo(start);
  }
  while (i <= start.y + pixelSize) {
    drawVector({ i % 2 == 0 ? start.x : start.x + pixelSize, i, 0});
    i++;
  }
}

void verticalFill(struct point start, uint8_t pixelSize) {
  uint8_t i = start.x;
  if (location != start) {
    moveTo(start);
  }
  while (i <= start.x + pixelSize) {
    drawVector({ i, i % 2 == 0 ? start.y : start.y + pixelSize, 0});
    i++;
  }
}

void drawCircle(struct point center, uint8_t radius) {
  for (uint16_t i = 0; i < 360; i++) {
    uint8_t x = center.x + sin((float)(i) * PI / 180) * radius;
    uint8_t y = center.y + cos((float)(i + 90) * PI / 180) * radius;
    if (!i) {
      moveTo({x, y});
      penDown();
    }
    moveX(x);
    moveY(y);
  }
}

void moveX(uint8_t x) {
  if (location.x < x) {          //Step one step forward on x-axis, if location on x-axis is less than target
#if REVERSE_X
    X_MOTOR.step(-1);
#else
    X_MOTOR.step(1);
#endif
    location.x++;
  } else if (location.x > x) {   //Step one step back on x-axis, if location on x-axis is higher than target
#if REVERSE_X
    X_MOTOR.step(1);
#else
    X_MOTOR.step(-1);
#endif
    location.x--;
  }
}

void moveY(uint8_t y) {
  if (location.y < y) {          //Step one step forward on y-axis, if location on y-axis is less than target
#if REVERSE_Y
    Y_MOTOR.step(-1);
#else
    Y_MOTOR.step(1);
#endif
    location.y++;
  } else if (location.y > y) {   //Step one step back on y-axis, if location on y-axis is higher than target
#if REVERSE_Y
    Y_MOTOR.step(1);
#else
    Y_MOTOR.step(-1);
#endif
    location.y--;
  }
}

void moveZ(uint8_t z) {
  if (z && !location.z) {        //Lift the pen up if Z is 1 and pen is down
    penUp();
  } else if (!z && location.z) { //Release the pen down if Z is 0 and pen is up
    penDown();
  }
}

/*
  Move the pen to target location while pen is held up
*/

void moveTo(struct point node) {
  penUp();
#if REVERSE_X
  X_MOTOR.step(location.x - node.x);
#else
  X_MOTOR.step(node.x - location.x);
#endif
#if REVERSE_Y
  Y_MOTOR.step(location.y - node.y);
#else
  Y_MOTOR.step(node.y - location.y);
#endif
  location.x = node.x;
  location.y = node.y;
}

void penUp() {
  servo.write(PEN_UP_ANGLE);
  location.z = 1;
  delay(SERVO_DELAY);
}

void penDown() {
  servo.write(PEN_DOWN_ANGLE);
  location.z = 0;
  delay(SERVO_DELAY);
}

/*
  Drive stepper motor pins low, so they can be moved freely and so they don't consume power
*/
void releaseMotors() {
  for (uint8_t i = 2; i < 10; i++) {
    digitalWrite(i, LOW);
  }
}
