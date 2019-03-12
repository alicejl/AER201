/** 
 * @file
 * @author Michael Ding
 * @author Tyler Gamvrelis
 * 
 * @defgroup Arduino_I2C_PIC
 * @ingroup Demo
 * @brief Demonstration of Arduino-PIC interfacing via I2C. In this program,
 *        the PIC outputs keypad data to the Arduino, which forwards it to a
 *        PC. If a triple-A sequence is given on the keypad, then the PIC will
 *        display data entered into the serial monitor on the character LCD. To
 *        open the serial monitor, go to Tools -> Serial Monitor
 * 
 * Preconditions:
 * @pre PIC-Arduino link switches are enabled (ON) for A4 and A5 of the Arduino
 *      Nano (RC4 and RC3 for the primary PIC, respectively)
 * @pre PIC and Arduino Nano agree on the Arduino Nano's slave address (i.e. the
 *      same slave address is used in software)
 * @pre A PC is connected if the Arduino Nano serial monitor is to be used. Note that
 *      the serial monitor baud rate must be 9600 for this program
 */

#include <Wire.h>

//CONSTANTS AND DEFINITIONS
const int trigPin = 3;
const int echoPin = 4;
const int trigPin2 = 5;
const int echoPin2 = 6;
const int encoderPin = 2;

// VARIABLES
//ultrasonic

//Encoder
int curr;
int last;
int encodeCount=0;
int enableEncoder=1;


//I2C
volatile char ID;
volatile byte sendArray[2];
volatile int counter=0;//send byte counter




void setup(){
    //SETUP ULTRASONIC SENSOR
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin2, INPUT); // Sets the echoPin as an Input


    //SETUP ENCODER
    pinMode(encoderPin, INPUT);//Sets the encodePin as an Input
    
    //SETUP I2C
    Wire.begin(8); // Join I2C bus with address 8
  
    // Register callback functions
    Wire.onReceive(receiveEvent); // Called when this slave device receives a data transmission from master
    Wire.onRequest(requestEvent); // Called when master requests data from this slave device
  
    // Open serial port to PC (hardware UART)
    Serial.begin(9600);      
}


int i;

void loop(){
      // put your main code here, to run repeatedly:
  curr = digitalRead(encoderPin);
  if(last == LOW && curr == HIGH && enableEncoder){
      encodeCount += 1;//distance in millimeters
      Serial.println(encodeCount);
  }
  last = curr;
}

/** @brief Callback for when the master transmits data */
void receiveEvent(void){
    //static uint8_t buf[3] = {0};
    //static uint8_t counter = 0;
    
    
    ID = (char)Wire.read(); // Receive byte
    Serial.println(ID); // Print to serial output as char (ASCII representation)

    int val;
    
    if (ID=='1'){//ultrasonic sensor
      val=ultraSonic();
    }
    else if (ID=='2'){//distance sensor
      val=encodeCount*11;//distance traveled in mm
    }
    else if (ID == '3'){//colour sensor response
      val=detectBall();
    }
    else if(ID=='4'){//stop encoder
      enableEncoder=0;
    }
    else if(ID=='5'){//restart encoder
      enableEncoder=1;
    }
    
    sendArray[1] = (val>>8)&0xFF;
    sendArray[2] = val&0xFF;
    
}

/** @brief Callback for when the master requests data */
void requestEvent(void){
    if (counter==0){
        Wire.write(sendArray[1]); // Respond with message of 1 byte
        counter=1;
    }
    else{
        Wire.write(sendArray[2]);
        counter=0;
    }
    
    //Serial.println(sendArray[1]);
    //Serial.println(sendArray[2]);
    //incomingByte = 0; // Clear output buffer
}

int ultraSonic(void){
  long duration;
  int distance=4000;

  while (distance>500){//filters out spikes in data
    // Clears the trigPin
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
  
    // Sets the trigPin on HIGH state for 10 micro seconds
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
  
    // Reads the echoPin, returns the sound wave travel time in microseconds
    duration = pulseIn(echoPin, HIGH);
    // Calculating the distance
    distance= duration/58;
  }
  Serial.println(distance);
  return distance;
}

int detectBall(void){
  long duration;
  int distance;
  long duration2;
  int distance2;
  // Clears the trigPin
  digitalWrite(trigPin, LOW);
  digitalWrite(trigPin2, LOW);
  delayMicroseconds(2);
  
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  digitalWrite(trigPin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  digitalWrite(trigPin2, LOW);
  
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  duration2 = pulseIn(echoPin2, HIGH);
  
  // Calculating the distance
  distance= duration/58;
  distance2= duration2/58;
  
  // Prints the distance on the Serial Monitor
  int difference=distance2-distance;
  if (difference>6){
    Serial.print("Ball");
    return 1;
  }
  else{
    Serial.print("No Ball");
    return 0;
  }
  return -1;
}