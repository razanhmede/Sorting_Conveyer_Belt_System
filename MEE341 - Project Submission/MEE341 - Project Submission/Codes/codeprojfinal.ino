#include <Servo.h>

Servo servo;

// Motor A connections
int enA_B = 9;
int in1 = 8;
int in2 = 7;
// Motor B connections
int in3 = 5;
int in4 = 4;

const int metal = 3; //pin for inductive proximity sensor
bool isMetal = false, isPlastic = true, isWood = false; // 30 ; 90 ; 150
bool systemOn = false, firstIteration = true;
const int pushButton = 2;
int trigPin = 13;    // TRIG pin
int echoPin = 12;    // ECHO pin
float duration_us, distance_cm; //values for ultrasonic

void setup() {
  pinMode(metal, INPUT);
  pinMode(pushButton, INPUT);
  Serial.begin(9600);
  servo.attach(6);
  servo.write(90);

  // configure the trigger pin to output mode
  pinMode(trigPin, OUTPUT);
  // configure the echo pin to input mode
  pinMode(echoPin, INPUT);

  pinMode(enA_B, OUTPUT);
	pinMode(in1, OUTPUT);
	pinMode(in2, OUTPUT);
	pinMode(in3, OUTPUT);
	pinMode(in4, OUTPUT);
	
	// Turn off motors - Initial state
	digitalWrite(in1, LOW);
	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, LOW);
}

void detectMaterial(){
    detectMetal();
    //computerVision();    
}

void detectMetal(){
  int object = digitalRead(metal);
    if(object==HIGH){
        Serial.println("Metal Object Not Detected");
        delay(50);
    }
    else{
        isMetal = true;
        isWood = false;
        isPlastic = false;
        Serial.println("Metal Object Detected");
        delay(50);
    }
}

void computerVision(){
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (command == 'b') {
      // Read and discard any remaining characters in the serial buffer
      while (Serial.available() > 0) {
        Serial.read();
      }
      
      Serial.println("Wood is detected");
      isWood = true; 
      isMetal = false;
      isPlastic = false; 
    } else {
      // Discard any remaining characters in the serial buffer
      while (Serial.available() > 0) {
        Serial.read();
      }
      
      Serial.println("Wood not detected");
    }
    
    delay(500); // Add a small delay to prevent rapid polling of serial buffer
  }
  }




void rampPosition(){
  if(isMetal){
    servo.write(70);
  }
  else if(isPlastic){
    servo.write(90);
  }
  else{
    servo.write(110);
  }
}

void conveyor(bool conveyorOn){
	if (conveyorOn) {
    // Set motors to maximum speed
  	// For PWM maximum possible values are 0 to 255
    analogWrite(enA_B, 255);
	
	  // Turn on motor A & B
	  digitalWrite(in1, LOW);
	  digitalWrite(in2, HIGH);
	  digitalWrite(in3, LOW);
	  digitalWrite(in4, HIGH);
	  delay(2000);    
  }
  else {
    analogWrite(enA_B, 0);
	
	  // Turn on motor A & B
	  digitalWrite(in1, HIGH);
	  digitalWrite(in2, HIGH);
	  digitalWrite(in3, HIGH);
	  digitalWrite(in4, HIGH);
  }
}

void ultrasonic(){
  // generate 10-microsecond pulse to TRIG pin
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // measure duration of pulse from ECHO pin
  duration_us = pulseIn(echoPin, HIGH);

  // calculate the distance
  distance_cm = 0.017 * duration_us;

  // print the value to Serial Monitor
  Serial.print("distance: ");
  Serial.print(distance_cm);
  Serial.println(" cm");

  if(distance_cm<7){
    isMetal = false;
    isPlastic = true;
    isWood = false;
    delay(1000);
    Serial.println("An Object has been Sorted");
    delay(2000);
    //gripper();
  }
  
}


void loop() {
  delay(200);
  int PBstate = digitalRead(pushButton); //Standby button
  Serial.println("Pushbutton state: " + PBstate);
  //Serial.println("Plastic state: " + isPlastic);
  //Serial.println("Wood state: " + isWood);
  //Serial.println("Metal state: " + isMetal); 
  
  detectMaterial();
  
  if(systemOn){
    Serial.println("System is ON!");
    //Intializing system
    if(firstIteration){
      firstIteration = false;
      delay(10000);
      Serial.println("Gripper Activated");//gripper();
    }
    //turn on conveyor belt
    conveyor(true);
    detectMaterial();

    rampPosition(); //adjust the ramp angle
    ultrasonic(); //detect presence of object at the last position and activate gripper

    if(PBstate == HIGH){ //if standby, turn off
      systemOn = false;
    }
  }
  else {
    Serial.println("System is OFF!");
    conveyor(false); //turn off motors
    //delay(500);    
    if(PBstate == HIGH){ //if standby turn on
      systemOn = true;
    }
  }
}



