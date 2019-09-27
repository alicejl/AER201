/** 
 * @file
 * @author Michael Ding
 * @author Tyler Gamvrelis
 * @author Alice Liang
 * 
 * @defgroup Arduino_I2C_PIC
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
const int trigPin = 4;
const int echoPin = 5;
const int trigPin2 = 6;
const int echoPin2 = 7;
const int encoderPin1 = 2;

// VARIABLES
//ultrasonic

//Encoder

volatile byte tempCount1=0;
volatile byte count1=0;
volatile byte tempCount2=0;
volatile byte count2=0;
int distance=0;
int enableEncoder=1;


//I2C
volatile char ID;
volatile byte sendArray[2];
volatile int counter=1;//send byte counter


//Detect Ball
int ballS=0;

//Timer
unsigned long startTime;

void setup(){
    //SETUP ULTRASONIC SENSOR
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    pinMode(trigPin2, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin2, INPUT); // Sets the echoPin as an Input


    //SETUP ENCODER
    attachInterrupt(digitalPinToInterrupt(encoderPin1), encoder1, RISING);
    attachInterrupt(digitalPinToInterrupt(encoderPin2), encoder2, RISING);
    
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
  
}

void encoder1(void){
  if ((tempCount1==254)&&(enableEncoder)){
    tempCount1=0;
    count1=count1+1;
  }
  else if (enableEncoder){
    tempCount1=tempCount1+1;
  }
}


/** @brief Callback for when the master transmits data */
void receiveEvent(void){
    int val=0;
    
    Serial.print("call ");
    ID = (char)Wire.read(); // Receive byte
    Serial.println(ID); // Print to serial output as char (ASCII representation)
    
    if (ID=='1'){//ultrasonic sensor
      val=ultraSonic();
    }
    else if (ID=='2'){//distance sensor
      val=count1*255+tempCount1;//encoderCount1;//distance traveled in cm
      Serial.print("distance");
      Serial.println(val);
    }
    else if (ID == '3'){//detect ball
      //val=ballS;
    }
    else if(ID=='4'){//stop encoder
      enableEncoder=0;
    }
    else if(ID=='5'){//restart encoder
      enableEncoder=1;
    }
    else if (ID=='6'){
      val=detectBall();
    }
    else if (ID=='7'){
    }
    else if(ID=='8'){
      tempCount1=0;
      count1=0;
      tempCount2=0;
      count2=0;
      enableEncoder=1;
      counter=1;
      startTime=millis();
    }
    else if (ID=='9'){
      unsigned long temp=millis();
      temp=temp-startTime;
      temp=temp/1000;
      val=(int)temp;
    }
    else if (ID=='A'){
      val=count2*255+tempCount2;
    }
    else{
      //placeholder
    }
    Serial.print("\val: ");
    Serial.println(val);
    sendArray[1] = (val>>8)&0xFF;
    sendArray[2] = val&0xFF;
    Serial.print("sendAB4\n");
    Serial.println(sendArray[1]);
    Serial.println(sendArray[2]);
}

/** @brief Callback for when the master requests data */
void requestEvent(void){
    Serial.print("SENDING");
    if (counter==0){
        Wire.write(sendArray[1]); // Respond with message of 1 byte
        counter=1;
        Serial.print("sendA\n");
        Serial.println(sendArray[1]);
    }
    else{
        Wire.write(sendArray[2]);
        counter=0;
        Serial.print("sendA");
        Serial.println(sendArray[2]);
        
    }
}

int ultraSonic(void){
  long duration;
  int distance=4000;
  int i;

  while (distance>1000){//filters out spikes in data
    distance=0;
    for (i=0;i<2;i++){
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
      distance=  (duration/58) +distance;
    }
    distance=distance/2;
  }
  Serial.println(distance);
  return distance;
}

int detectBall(void){
  long duration;
  int distance=4000;
  long duration2;
  int distance2=4000;
  int i;
  int j;
  int diff=1;


  distance=ultraSonic();
  
  for (i=0;i<4;i++){        
    //sensor 2
  
    digitalWrite(trigPin2, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin2, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin2, LOW);
    duration = pulseIn(echoPin2, HIGH);  
    distance2 = distance2 + duration/58;
  }

  distance2=distance2/4;

  // Prints the distance on the Serial Monitor
  
  Serial.print("sensor1 = ");
  Serial.print(distance);
  Serial.print(" sensor2 = ");
  Serial.println(distance2);
  
  diff=distance-distance2;
  
  if (diff>=5){
    Serial.print("B");
    ballS=0;
  }
  else{
    Serial.print("NB");
    ballS=1;
  }

  Serial.print("ballS");
  Serial.println(ballS);
  return ballS;
}
