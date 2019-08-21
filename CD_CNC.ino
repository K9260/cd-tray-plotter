#include <Stepper.h>
#include <Servo.h>

#define REVERSE_X      1
#define REVERSE_Y      0
#define STEPS_PER_REV  20
#define MAX_STEPS      250
#define PEN_UP_ANGLE   40
#define PEN_DOWN_ANGLE 80
#define SERVO_DELAY   (PEN_DOWN_ANGLE - PEN_UP_ANGLE) * 10

#define SERVO_PIN      10

Stepper X_MOTOR(STEPS_PER_REV, 6, 7, 8, 9);
Stepper Y_MOTOR(STEPS_PER_REV, 2, 3, 4, 5);
Servo servo;

struct point {
  uint8_t x, y, z;
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
  {250, 250, 0}
};


struct point location = {0, 0, 0};

void setup() {
  Serial.begin(9600);
  X_MOTOR.setSpeed(1000);
  Y_MOTOR.setSpeed(1000);
  servo.attach(SERVO_PIN);
  drawShape(shape, 4, 2);
  //drawShape(triangle, 3, 1);
  //drawShape(rectangle, 4, 1);
  //drawShape(line, 2, 5);
  moveTo({0, 0});
  releaseMotors();
}

void loop() {

}

void drawShape (struct point nodes[], uint8_t len, uint8_t rounds) {
  for (uint8_t j = 0; j < rounds; j++) {  //How many times to draw the same shape (to make the pen markings more visible)
    moveTo(nodes[0]);                     //Go to first node with pen up
    for (uint8_t i = 0; i < len + 1; i++) {
      drawVector(nodes[i % len]);
    }
  }
  penUp();
}

void drawVector(struct point node) {
  uint8_t xDistance = abs(node.x - location.x);          //Distance to travel on x-axis
  uint8_t yDistance = abs(node.y - location.y);          //Distance to travel on y-axis
  char longerAxis = xDistance > yDistance ? 'x' : 'y';   //Which axis has longer distance to travel
  float scale = (longerAxis == 'x') ? (float)(xDistance) / (float)(yDistance) : (float)(yDistance) / (float)(xDistance);

  while (location.x != node.x || location.y != node.y) { //Loop until motors reach target node
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
  X_MOTOR.step(node.x - location.x);
  Y_MOTOR.step(node.y - location.y);
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

