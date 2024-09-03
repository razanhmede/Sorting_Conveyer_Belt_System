#include <Servo.h>

// Define Servo objects for controlling the arm, gripper, and base rotation
Servo servo1;    // Servo for joint 1
Servo servo2;    // Servo for joint 2
Servo gripper;   // Servo for the gripper
Servo base;      // Servo for the base rotation

// Function prototypes
void inv_kinematics(float x, float y);
void openGripper();
void closeGripper();
void rotateBase();

// Constants for arm lengths
const float L1 = 12;  // Length of the first arm segment
const float L2 = 17;  // Length of the second arm segment

void setup() {
  // Initialize serial communication at 9600 bits per second
  Serial.begin(9600);
  delay(1000);  // Short delay after opening communication

  // Attach the servos to specified pins
  servo1.attach(3);
  servo2.attach(5);
  gripper.attach(6);
  base.attach(9);  // Attach the base servo to pin 7

  // Move servos to initial position and open the gripper
  initialPosition();
}

void loop() {
  // Coordinates of the target point
  float x = 10, y = 15;
  initialPosition();
  // Ensure gripper is open before moving to the target
  openGripper();
  
  // Compute inverse kinematics for the given target point
  inv_kinematics(x, y);
  
  // Wait for the arm to reach the target position
  delay(1000);
  
  // Close the gripper once the arm has reached the target position
  closeGripper();
  
  // Rotate base to place object in container behind
  rotateBase();
}

void rotateBase() {
  // Rotate the base 180 degrees to place the object
  base.write(180);
  Serial.println("Base rotated to place object.");
  delay(1000);  // Wait for rotation to complete

  // Open the gripper to release the object
  openGripper();
  Serial.println("Object placed. Gripper is open.");

  // Optionally, rotate back to the initial position
  initialPosition();
}

void inv_kinematics(float x, float y) {
  // Compute the distance from the origin to the point
  float d = sqrt(pow(x, 2) + pow(y, 2));
  Serial.print("d: ");
  Serial.println(d);

  // Compute angle to point (in degrees) using atan2 for full quadrant coverage
  float theta = degrees(atan2(y, x));
  Serial.print("theta: ");
  Serial.println(theta);

  // Check if the point is within reach
  if ((L1 + L2) >= d && abs(L1 - L2) <= d) {
    // Calculate joint angles using the cosine law
    float beta = degrees(acos((pow(L1, 2) + pow(L2, 2) - pow(d, 2)) / (2 * L1 * L2)));
    float alpha = degrees(acos((pow(L1, 2) + pow(d, 2) - pow(L2, 2)) / (2 * L1 * d)));

    float theta1 = theta + alpha;
    float theta2 = abs(90-beta);

    servo1.write(theta1);
    servo2.write(theta2);

    Serial.println("Arm moved to position.");
  } else {
    Serial.println("Position out of reach.");
    // Move servos to initial position if target is out of reach
    initialPosition();
  }
}

void initialPosition() {
  servo1.write(90);
  Serial.println("Servo 1 is in initial position");
  servo2.write(0);
  Serial.println("Servo 2 is in initial position");
  base.write(0);  // Set base to initial position
  Serial.println("Base is in initial position");
  openGripper();  // Open the gripper in the initial position
  Serial.println("Gripper is in initial position");
}

void openGripper() {
  gripper.write(150);  // Assuming 0 degrees is fully open, adjust as needed
  Serial.println("Gripper is open.");
}

void closeGripper() {
  gripper.write(10);  // Assuming 180 degrees is fully closed, adjust as needed
  servo1.write(90);
  servo2.write(0);
  Serial.println("Gripper is closed.");
}
