#include <Servo.h>

#define redPin 10
#define bluePin 9
#define greenPin 8

Servo myServo;

char dataReceive;

int wrongPasswordNumber;

void setup(){
  
  // Setup LED RGB
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  // Setup Piezo
  pinMode(4, OUTPUT);
  
  // Setup Servo
  myServo.attach(3);
  
  wrongPasswordNumber = 0;
  
  Serial.begin(9600);
}

void loop(){
  
  myServo.write(90);
  
  if(Serial.available()){
	char dataReceive = Serial.read();
    
    // Incorrect password
    if(dataReceive == '1'){
      redLed();
      
    // Correct password
    } else if(dataReceive == '2'){
      greenLed();
      myServo.write(0);
      
    // Admin mode
    } else if(dataReceive == '3'){
      yellowLed();
      
    // User mode
    } else if(dataReceive == '4'){
      blueLed();
    
    // Turn on Alarm
    } else if(dataReceive == '5'){
      digitalWrite(4, HIGH);
      
    // Turn off Alarm
    } else if(dataReceive == '6'){
      digitalWrite(4, LOW);
    }
  }
}

void redLed(){
	digitalWrite(redPin,HIGH);
  	digitalWrite(bluePin,LOW);
  	digitalWrite(greenPin,LOW);
}

void greenLed(){
	digitalWrite(redPin, LOW);
  	digitalWrite(bluePin, LOW);
  	digitalWrite(greenPin, HIGH);
}

void yellowLed(){
	digitalWrite(redPin, HIGH);
  	digitalWrite(bluePin, LOW);
  	digitalWrite(greenPin, HIGH);
}

void blueLed(){
	digitalWrite(redPin, LOW);
  	digitalWrite(bluePin, HIGH);
  	digitalWrite(greenPin, LOW);
}