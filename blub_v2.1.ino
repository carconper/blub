// Libraries
#include "Timer.h"

// Define pins to be used
// -> OUT pins (lighting)
const int YELLOW1 = 3;  // DGTL PWM OUT
const int YELLOW2 = 5;  // DGTL PWM OUT
const int ORANGE = 9;   // DGTL PWM OUT
const int BLUE = 10;    // DGTL PWM OUT
const int WHITE = 11;   // DGTL PWM OUT

//const int red = 9;      // DGTL PWM OUT
//const int green = 6;    // DGTL PWM OUT
//const int yellow = 5;   // DGTL PWM OUT
//const int blue = 3;     // DGTL PWM OUT

// -> IN pins (inputs)
const int cloudPin = 12;// DGTL OUT  
const int soundPin = 0; // ANLG IN

// State Machine -- States
short state = 0;            // To keep the track of the state 
const short OFF = 0;        // default State
const short DAY = 1;        // Day State
const short DAYUP = 8;      // Day -- Waking up State
const short DAYON = 9;      // Day -- On state
const short DAYWEATHER = 2; // Day -- Show Weather State
const short DAYTRAFFIC = 3; // Day -- Show Traffic State
const short NIGHT = 4;      // Night State
const short NIGHTAWAKE = 5; // Night -- Awake State
const short NIGHTDOWN = 6;  // Night -- Falling Sleep State
const short NIGHTSLEEP = 7; // Night -- Sleep State

// Timers
Timer tAwake;             // Keeps control of awake Time on night state
Timer tSleep;             // Keeps dimm down time
Timer tWakeup;            // Triggers Day State
//Timer tTraffic;           // Keeps show traffic time
//Timer tWeather;           // Keeps show weather time
Timer tDebug;             // Print debugging info into Serial

// Normal flow
int time_alarm;

// Debug
String toPrint = "State: ";

// Auxiliaries
boolean alternate;
short bright_yell1 = 0;
short bright_yell2 = 0;
short bright_orange = 0;
short bright_blue = 0;
short bright_white = 0;

//int bright_red = 0;
//int bright_green = 0;
//int bright_blue = 0;
//int bright_yellow = 0;

void setup() {
  // put your setup code here, to run once:

  pinMode(cloudPin, INPUT);
  Serial.begin(9600); // You can uncomment this for monitoring

  pinMode(YELLOW1, OUTPUT);
  pinMode(YELLOW2, OUTPUT);
  pinMode(ORANGE, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(WHITE, OUTPUT);

  analogWrite(YELLOW1, bright_yell1);
  analogWrite(YELLOW2, bright_yell2);
  analogWrite(ORANGE, bright_orange);
  analogWrite(BLUE, bright_blue);
  analogWrite(WHITE, bright_white);
  
  //pinMode(red, OUTPUT);
  //pinMode(green, OUTPUT);
  //pinMode(yellow, OUTPUT);
  //pinMode(blue, OUTPUT);

  //analogWrite(red, bright_red);
  //analogWrite(green, bright_green);
  //analogWrite(yellow, bright_yellow);
  //analogWrite(blue, bright_blue);
  
  alternate = false;

  // Set the alarm time
  time_alarm = 1000 ;

}

/* 
NAME: setInitState()
IN:   NONE
OUT:  sm_state
DESC: This function will be call at every loop and will determine 
      the State Machine status based on the available different 
      inputs
*/
int setInitState(boolean cloudState, int soundState) {

  int sm_state = 0;

  // DEBUG_CODE
  cloudState=true;
  soundState=101;
  // DEBUG_END
  
  if (cloudState) {

    if (soundState > 100) {
      sm_state = NIGHTAWAKE;
    } else {
      sm_state = NIGHTDOWN;
    }
    
  } else {
    sm_state = DAY;
  }

  return sm_state;

}



void upBrightness() {

  // First check yell1 status and act when it completely
  //  switches off
  if (bright_yell1 >= 255) {

    bright_yell1 = 255;
    analogWrite(YELLOW1, bright_yell1);
    
    // Second check yell2 status until completely off
    if (bright_yell2 >= 255) {

      bright_yell2 = 255;
      analogWrite(YELLOW2, bright_yell2);
      
      // Last check orange status until off
      if (bright_white >= 255) {

        bright_white = 255;
        analogWrite(WHITE, bright_white);
        
        state = DAYON;
        return;  
        
      } else {

        analogWrite(WHITE, bright_white);
        
        bright_white = bright_white + 100;
        
        toPrint = "Message " ;
        toPrint += state;
        toPrint += "|";
        toPrint += bright_white ;
        printSerial(toPrint);
        
      } // if white
    
    } else {

      analogWrite(YELLOW2, bright_yell2);
      
      bright_yell2 = bright_yell2 + 100;
      
      toPrint = "Message " ;
      toPrint += state;
      toPrint += "|";
      toPrint += bright_yell2 ;
      printSerial(toPrint);
        
    } // if yell2
    
  } else {

    analogWrite(YELLOW1, bright_yell1);
    bright_yell1 = bright_yell1 + 100;
    
    toPrint = "Message " ;
    toPrint += state;
    toPrint += "|";
    toPrint += bright_yell1 ;
    printSerial(toPrint);
        
  } // if yell1


  
}



void downBrightness() {

  
  // First check yell1 status and act when it completely
  //  switches off
  if (bright_yell1 <= 0) {

    bright_yell1 = 0;
    analogWrite(YELLOW1, bright_yell1);
    
    // Second check yell2 status until completely off
    if (bright_yell2 <= 0) {

      bright_yell2 = 0;
      analogWrite(YELLOW2, bright_yell2);
      
      // Last check orange status until off
      if (bright_orange <= 0) {

        bright_orange = 0;
        analogWrite(ORANGE, bright_orange);
        
        state = NIGHTSLEEP;
        return;  
        
      } else {

        analogWrite(ORANGE, bright_orange);
        
        bright_orange = bright_orange - 100;
        
        toPrint = "Message " ;
        toPrint += state;
        toPrint += "|";
        toPrint += bright_orange ;
        printSerial(toPrint);
        
        
      } // if orange
    
    } else {

      analogWrite(YELLOW2, bright_yell2);
      
      bright_yell2 = bright_yell2 - 100;
      toPrint = "Message " ;
      toPrint += state;
      toPrint += "|";
      toPrint += bright_yell2 ;
      printSerial(toPrint);

      
    } // if yell2
    
  } else {

    analogWrite(YELLOW1, bright_yell1);
    
    bright_yell1 = bright_yell1 - 100;
    toPrint = "Message " ;
    toPrint += state;
    toPrint += "|";
    toPrint += bright_yell1 ;
    printSerial(toPrint);
   
    
    
  } // if yell1
    
}


void setState(short stt) {

  toPrint = "Setting State to: ";
  toPrint = toPrint + stt ;
  Serial.print(toPrint);
  state = stt;

  return;
}



void setNightdown() {

  Serial.print("Setting Night Down State");
  state = NIGHTDOWN;
  
  return;
}



void setWakeup() {

  Serial.print("Setting Day State");
  state = DAYUP;

  return;
  
}

// DEBUG_CODE  
void printSerial(String msg) {
  
  Serial.println(msg);

}
// DEBUG_END

void loop() {
  // put your main code here, to run repeatedly:
  
  boolean cloudState = digitalRead(cloudPin); // Read the signal received from cloud
  int soundState = analogRead(soundPin); // Read sound sensor’s value
  int downEvent ;
  int upEvent ;

  //Serial.println(soundState);

/*  if (alternate == true) {
    alternate = false;
  } else {
    alternate = true;
  }
*/

  if (state == OFF) {
    state = setInitState(cloudState, soundState) ;
  }



  // DEBUG_CODE
  //tDebug.every(2*1000,printSerial);
  // DEBUG_END

  //tDebug.update();
  tSleep.update();
  tAwake.update();
  tWakeup.update();
  
  

  switch (state) {

    case NIGHT:     // Transition State 
      // wait for timers to expire
      break;
      
    case NIGHTAWAKE:
      // Keep Light on
      // Set pattern night-awake
      bright_yell1 = 255;
      bright_yell2 = 255;
      bright_orange = 255;

      // Switch on BLUB
      analogWrite(YELLOW1, bright_yell1);
      analogWrite(YELLOW2, bright_yell1);
      analogWrite(ORANGE, bright_orange);
      
      // Start Timer tAwake
      tAwake.after(10*1000,setNightdown);

      // Go to NIGHT waiting state
      state = NIGHT;
      
      break;
      
    case NIGHTDOWN:
      // Start Sleepdown timer tSleep
      // Dim down every 5 seconds
      downEvent = tSleep.every(3*1000,downBrightness);

      // Go to NIGHT waiting state
      state = NIGHT;
      
      break;
      
    case NIGHTSLEEP:
      
      // Stop tSleep timer
      tSleep.stop(downEvent);

      // Start alarm to wake up (tWakeup timer)
      Serial.print("Setting timer for waking up!");
      tWakeup.after(time_alarm,setWakeup);

      // Go to NIGHT waiting state
      state = NIGHT;
      
      break;

    case DAYUP:

      // Start waking up timer
      // Dim up every 5 seconds
      upEvent = tWakeup.every(5*1000, upBrightness);

      // Move to waiting state DAY
      state = DAY ; 

      break;
      
    case DAYWEATHER:
    
      digitalWrite(YELLOW1, HIGH);

      //digitalWrite(red, LOW);
      //digitalWrite(green, LOW);
      digitalWrite(BLUE, LOW);
      break;
      
    case DAYTRAFFIC:
    
      digitalWrite(YELLOW1, HIGH);

      if (alternate == true) {
        digitalWrite(BLUE, HIGH);
        delay(500);  
      } else {
        digitalWrite(BLUE, LOW);
        delay(500);
      }
      
      //digitalWrite(RED, LOW);
      //digitalWrite(GREEN, LOW);
      
      break;
      
    case DAY:
    
      break;

    case DAYON:

      Serial.print("DAYON") ;
      // Stop tWakeup timer
      tWakeup.stop(upEvent);
      
      state = DAY;

      toPrint = "Message " ;
      toPrint += state;
      toPrint += "|";
      toPrint += bright_blue ;
      printSerial(toPrint);

      analogWrite(YELLOW1, 0);
      analogWrite(YELLOW2, 0);
      analogWrite(WHITE, 0);
      analogWrite(ORANGE, 0);
      analogWrite(BLUE, 200);

      break;
      
    case OFF:

      Serial.print("OFF") ;
      analogWrite(YELLOW1, 0);
      analogWrite(YELLOW2, 0);
      analogWrite(WHITE, 0);
      analogWrite(ORANGE, 0);
      analogWrite(BLUE, 200);
      
      break;
      
    default:
      
      digitalWrite(YELLOW1, LOW);
      digitalWrite(BLUE, LOW);
      
      break;
  }

}
